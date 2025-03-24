#include "myrtx/collections/hash_table.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* Konstanten für die Hash-Tabelle */
#define MYRTX_DEFAULT_CAPACITY 16
#define MYRTX_DEFAULT_LOAD_FACTOR 0.75f

/* Status eines Hash-Tabelleneintrags */
typedef enum {
    MYRTX_HASH_ENTRY_EMPTY = 0,
    MYRTX_HASH_ENTRY_OCCUPIED,
    MYRTX_HASH_ENTRY_DELETED
} myrtx_hash_entry_status_t;

/* Eintrag in der Hash-Tabelle */
typedef struct {
    void* key;
    size_t key_size;
    void* value;
    size_t value_size;
    myrtx_hash_entry_status_t status;
    uint32_t hash;
} myrtx_hash_entry_t;

/* Hash-Tabellen-Struktur */
struct myrtx_hash_table_t {
    myrtx_hash_entry_t* entries;
    size_t capacity;          /* Kapazität (Anzahl der möglichen Einträge) */
    size_t size;              /* Anzahl der tatsächlich belegten Einträge */
    size_t tombstones;        /* Anzahl der gelöschten Einträge (Grabsteine) */
    float load_factor;        /* Schwellenwert für Auslastung */
    myrtx_hash_function hash_func;
    myrtx_key_compare_function compare_func;
    myrtx_arena_t* arena;
};

/* Private Funktionen für Hash-Tabellen-Operationen */

/* Speicherallokationsfunktion, die entweder die Arena oder malloc verwendet */
static void* hash_table_malloc(myrtx_hash_table_t* table, size_t size) {
    if (table->arena) {
        return myrtx_arena_alloc(table->arena, size);
    } else {
        return malloc(size);
    }
}

/* Speicherfreigabefunktion für malloc-basierte Speicherzuweisungen */
static void hash_table_free(myrtx_hash_table_t* table, void* ptr) {
    if (!table->arena) {
        free(ptr);
    }
    /* Bei Arena-Allokation ist keine Freigabe einzelner Elemente notwendig */
}

/* Findet die nächsthöhere Potenz von 2 */
static size_t next_power_of_2(size_t n) {
    size_t power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

/* Erstellt einen neuen Hash-Tabelleneintrag */
static myrtx_hash_entry_t create_entry(myrtx_hash_table_t* table, 
                                     const void* key, size_t key_size,
                                     const void* value, size_t value_size, 
                                     uint32_t hash) {
    myrtx_hash_entry_t entry;
    
    /* Speicher für Schlüssel und Wert allozieren und kopieren */
    entry.key_size = key_size;
    entry.value_size = value_size;
    entry.status = MYRTX_HASH_ENTRY_OCCUPIED;
    entry.hash = hash;
    
    /* Schlüssel kopieren */
    entry.key = hash_table_malloc(table, key_size);
    if (!entry.key) {
        entry.status = MYRTX_HASH_ENTRY_EMPTY;
        return entry;
    }
    memcpy(entry.key, key, key_size);
    
    /* Wert kopieren */
    entry.value = hash_table_malloc(table, value_size);
    if (!entry.value) {
        if (!table->arena) {
            free(entry.key);
        }
        entry.status = MYRTX_HASH_ENTRY_EMPTY;
        return entry;
    }
    memcpy(entry.value, value, value_size);
    
    return entry;
}

/* Berechnet den Index für einen Hash in der Tabelle mit linearer Sondierung */
static size_t get_index(uint32_t hash, size_t capacity, size_t probe) {
    return (hash + probe) % capacity;
}

/* Findet den Index eines Eintrags oder eine freie Stelle für Einfügungen */
static size_t find_entry(const myrtx_hash_table_t* table, 
                        const void* key, size_t key_size, uint32_t hash,
                        bool* found) {
    size_t index, tombstone_index = SIZE_MAX;
    
    /* Lineare Sondierung */
    for (size_t i = 0; i < table->capacity; i++) {
        index = get_index(hash, table->capacity, i);
        
        /* Leerer Slot: Ende der Suche */
        if (table->entries[index].status == MYRTX_HASH_ENTRY_EMPTY) {
            *found = false;
            /* Wenn wir einen Grabstein gefunden haben, verwenden wir diesen für Einfügungen */
            return tombstone_index != SIZE_MAX ? tombstone_index : index;
        }
        
        /* Grabstein merken für potenzielle Einfügungen */
        if (table->entries[index].status == MYRTX_HASH_ENTRY_DELETED) {
            if (tombstone_index == SIZE_MAX) {
                tombstone_index = index;
            }
            continue;
        }
        
        /* Besetzter Slot: Prüfen, ob es der gesuchte Schlüssel ist */
        if (table->entries[index].hash == hash && 
            table->compare_func(table->entries[index].key, table->entries[index].key_size,
                              key, key_size)) {
            *found = true;
            return index;
        }
    }
    
    /* Tabelle voll, aber mit Grabsteinen */
    *found = false;
    return tombstone_index != SIZE_MAX ? tombstone_index : 0;
}

/* Vergrößert die Hash-Tabelle und ordnet alle Einträge neu an */
static bool resize_hash_table(myrtx_hash_table_t* table, size_t new_capacity) {
    if (new_capacity < MYRTX_DEFAULT_CAPACITY) {
        new_capacity = MYRTX_DEFAULT_CAPACITY;
    }
    
    /* Neue Einträge-Array allozieren */
    myrtx_hash_entry_t* new_entries = hash_table_malloc(table, sizeof(myrtx_hash_entry_t) * new_capacity);
    if (!new_entries) {
        return false;
    }
    
    /* Alle Einträge als leer markieren */
    for (size_t i = 0; i < new_capacity; i++) {
        new_entries[i].status = MYRTX_HASH_ENTRY_EMPTY;
    }
    
    /* Alte Einträge temporär speichern */
    myrtx_hash_entry_t* old_entries = table->entries;
    size_t old_capacity = table->capacity;
    
    /* Tabelle aktualisieren */
    table->entries = new_entries;
    table->capacity = new_capacity;
    table->tombstones = 0;
    
    /* Alte Einträge in die neue Tabelle einfügen */
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_entries[i].status == MYRTX_HASH_ENTRY_OCCUPIED) {
            bool found;
            size_t index = find_entry(table, old_entries[i].key, old_entries[i].key_size, 
                                     old_entries[i].hash, &found);
            
            /* Alten Eintrag an neue Position kopieren */
            table->entries[index] = old_entries[i];
        }
    }
    
    /* Alte Einträge-Array freigeben, wenn wir malloc verwenden */
    if (!table->arena) {
        free(old_entries);
    }
    
    return true;
}

/* Sicherheitsüberprüfung, ob die Hash-Tabelle vergrößert werden muss */
static bool ensure_capacity(myrtx_hash_table_t* table) {
    /* Prüfen, ob die Tabelle überlastet ist */
    float load = (float)(table->size + table->tombstones) / (float)table->capacity;
    
    if (load >= table->load_factor) {
        /* Neue Kapazität ist doppelt so groß */
        return resize_hash_table(table, table->capacity * 2);
    }
    
    return true;
}

/* Öffentliche API-Funktionen */

/* Erstellt eine neue Hash-Tabelle */
myrtx_hash_table_t* myrtx_hash_table_create(myrtx_arena_t* arena, 
                                          size_t initial_capacity,
                                          myrtx_hash_function hash_function, 
                                          myrtx_key_compare_function compare_function) {
    /* Parameter validieren */
    if (!hash_function || !compare_function) {
        return NULL;
    }
    
    /* Anfangskapazität auf Standardwert setzen, wenn 0 */
    if (initial_capacity == 0) {
        initial_capacity = MYRTX_DEFAULT_CAPACITY;
    } else {
        /* Auf nächste Potenz von 2 aufrunden */
        initial_capacity = next_power_of_2(initial_capacity);
    }
    
    myrtx_hash_table_t* table;
    
    /* Hash-Tabellen-Struktur allozieren */
    if (arena) {
        table = myrtx_arena_alloc(arena, sizeof(myrtx_hash_table_t));
    } else {
        table = malloc(sizeof(myrtx_hash_table_t));
    }
    
    if (!table) {
        return NULL;
    }
    
    /* Einträge-Array allozieren */
    myrtx_hash_entry_t* entries;
    if (arena) {
        entries = myrtx_arena_alloc(arena, sizeof(myrtx_hash_entry_t) * initial_capacity);
    } else {
        entries = malloc(sizeof(myrtx_hash_entry_t) * initial_capacity);
    }
    
    if (!entries) {
        if (!arena) {
            free(table);
        }
        return NULL;
    }
    
    /* Tabelle initialisieren */
    table->entries = entries;
    table->capacity = initial_capacity;
    table->size = 0;
    table->tombstones = 0;
    table->load_factor = MYRTX_DEFAULT_LOAD_FACTOR;
    table->hash_func = hash_function;
    table->compare_func = compare_function;
    table->arena = arena;
    
    /* Alle Einträge als leer markieren */
    for (size_t i = 0; i < initial_capacity; i++) {
        table->entries[i].status = MYRTX_HASH_ENTRY_EMPTY;
    }
    
    return table;
}

/* Gibt eine Hash-Tabelle frei */
void myrtx_hash_table_free(myrtx_hash_table_t* table, 
                         bool free_keys, 
                         bool free_values) {
    if (!table) {
        return;
    }
    
    /* Inhalte freigeben, wenn wir malloc verwendet haben */
    if (!table->arena) {
        /* Schlüssel und Werte freigeben, wenn angefordert */
        for (size_t i = 0; i < table->capacity; i++) {
            if (table->entries[i].status == MYRTX_HASH_ENTRY_OCCUPIED) {
                if (free_keys) {
                    free(table->entries[i].key);
                }
                if (free_values) {
                    free(table->entries[i].value);
                }
            }
        }
        
        /* Einträge-Array freigeben */
        free(table->entries);
        
        /* Hash-Tabellen-Struktur freigeben */
        free(table);
    }
}

/* Fügt einen Schlüssel-Wert-Paar zur Hash-Tabelle hinzu oder aktualisiert es */
bool myrtx_hash_table_put(myrtx_hash_table_t* table, 
                        const void* key, 
                        size_t key_size,
                        const void* value, 
                        size_t value_size) {
    if (!table || !key || !value) {
        return false;
    }
    
    /* Bei String-Schlüsseln die Länge ermitteln, wenn nicht angegeben */
    if (key_size == 0) {
        key_size = strlen(key) + 1;
    }
    
    /* Hash berechnen */
    uint32_t hash = table->hash_func(key, key_size);
    
    /* Sicherstellen, dass genug Kapazität vorhanden ist */
    if (!ensure_capacity(table)) {
        return false;
    }
    
    /* Position zum Einfügen suchen */
    bool found;
    size_t index = find_entry(table, key, key_size, hash, &found);
    
    /* Wenn Schlüssel bereits existiert, Wert aktualisieren */
    if (found) {
        /* Alten Wert freigeben, wenn wir malloc verwenden */
        if (!table->arena) {
            free(table->entries[index].value);
        }
        
        /* Neuen Wert allozieren */
        table->entries[index].value = hash_table_malloc(table, value_size);
        if (!table->entries[index].value) {
            return false;
        }
        
        /* Wert aktualisieren */
        memcpy(table->entries[index].value, value, value_size);
        table->entries[index].value_size = value_size;
        
        return true;
    }
    
    /* Neuen Eintrag erstellen */
    myrtx_hash_entry_t entry = create_entry(table, key, key_size, value, value_size, hash);
    
    /* Prüfen, ob der Eintrag erfolgreich erstellt wurde */
    if (entry.status != MYRTX_HASH_ENTRY_OCCUPIED) {
        return false;
    }
    
    /* Wenn wir einen Grabstein überschreiben, Tombstone-Zähler reduzieren */
    if (table->entries[index].status == MYRTX_HASH_ENTRY_DELETED) {
        table->tombstones--;
    }
    
    /* Eintrag in die Tabelle einfügen */
    table->entries[index] = entry;
    table->size++;
    
    return true;
}

/* Holt einen Wert aus der Hash-Tabelle */
bool myrtx_hash_table_get(const myrtx_hash_table_t* table, 
                        const void* key, 
                        size_t key_size,
                        void** value_out, 
                        size_t* value_size_out) {
    if (!table || !key || !value_out) {
        return false;
    }
    
    /* Bei String-Schlüsseln die Länge ermitteln, wenn nicht angegeben */
    if (key_size == 0) {
        key_size = strlen(key) + 1;
    }
    
    /* Hash berechnen */
    uint32_t hash = table->hash_func(key, key_size);
    
    /* Eintrag suchen */
    bool found;
    size_t index = find_entry(table, key, key_size, hash, &found);
    
    /* Prüfen, ob Schlüssel gefunden wurde */
    if (!found) {
        return false;
    }
    
    /* Wert zurückgeben */
    *value_out = table->entries[index].value;
    if (value_size_out) {
        *value_size_out = table->entries[index].value_size;
    }
    
    return true;
}

/* Prüft, ob ein Schlüssel in der Hash-Tabelle existiert */
bool myrtx_hash_table_contains_key(const myrtx_hash_table_t* table, 
                                 const void* key, 
                                 size_t key_size) {
    if (!table || !key) {
        return false;
    }
    
    /* Bei String-Schlüsseln die Länge ermitteln, wenn nicht angegeben */
    if (key_size == 0) {
        key_size = strlen(key) + 1;
    }
    
    /* Hash berechnen */
    uint32_t hash = table->hash_func(key, key_size);
    
    /* Eintrag suchen */
    bool found;
    find_entry(table, key, key_size, hash, &found);
    
    return found;
}

/* Entfernt einen Eintrag aus der Hash-Tabelle */
bool myrtx_hash_table_remove(myrtx_hash_table_t* table, 
                           const void* key, 
                           size_t key_size,
                           bool free_key, 
                           bool free_value) {
    if (!table || !key) {
        return false;
    }
    
    /* Bei String-Schlüsseln die Länge ermitteln, wenn nicht angegeben */
    if (key_size == 0) {
        key_size = strlen(key) + 1;
    }
    
    /* Hash berechnen */
    uint32_t hash = table->hash_func(key, key_size);
    
    /* Eintrag suchen */
    bool found;
    size_t index = find_entry(table, key, key_size, hash, &found);
    
    /* Prüfen, ob Schlüssel gefunden wurde */
    if (!found) {
        return false;
    }
    
    /* Schlüssel und Wert freigeben, wenn angefordert und wir malloc verwenden */
    if (!table->arena) {
        if (free_key) {
            free(table->entries[index].key);
        }
        if (free_value) {
            free(table->entries[index].value);
        }
    }
    
    /* Eintrag als gelöscht markieren (Grabstein) */
    table->entries[index].status = MYRTX_HASH_ENTRY_DELETED;
    table->size--;
    table->tombstones++;
    
    return true;
}

/* Gibt die Anzahl der Einträge in der Hash-Tabelle zurück */
size_t myrtx_hash_table_size(const myrtx_hash_table_t* table) {
    if (!table) {
        return 0;
    }
    
    return table->size;
}

/* Leert die Hash-Tabelle, entfernt alle Einträge */
void myrtx_hash_table_clear(myrtx_hash_table_t* table, 
                          bool free_keys, 
                          bool free_values) {
    if (!table) {
        return;
    }
    
    /* Schlüssel und Werte freigeben, wenn angefordert und wir malloc verwenden */
    if (!table->arena) {
        for (size_t i = 0; i < table->capacity; i++) {
            if (table->entries[i].status == MYRTX_HASH_ENTRY_OCCUPIED) {
                if (free_keys) {
                    free(table->entries[i].key);
                }
                if (free_values) {
                    free(table->entries[i].value);
                }
            }
            
            /* Eintrag als leer markieren */
            table->entries[i].status = MYRTX_HASH_ENTRY_EMPTY;
        }
    } else {
        /* Bei Arena-Allokation einfach alle Einträge als leer markieren */
        for (size_t i = 0; i < table->capacity; i++) {
            table->entries[i].status = MYRTX_HASH_ENTRY_EMPTY;
        }
    }
    
    /* Zähler zurücksetzen */
    table->size = 0;
    table->tombstones = 0;
}

/* FNV-1a Hash-Algorithmus für Strings */
uint32_t myrtx_hash_string(const void* key, size_t key_size) {
    const unsigned char* data = (const unsigned char*)key;
    uint32_t hash = 2166136261u;  /* FNV-1a Basis-Offset */
    
    /* Bei Länge 0 den String als null-terminiert behandeln */
    if (key_size == 0) {
        while (*data) {
            hash ^= *data++;
            hash *= 16777619u;  /* FNV-1a Prime */
        }
    } else {
        /* Angegebene Anzahl von Bytes hashen */
        for (size_t i = 0; i < key_size; i++) {
            hash ^= data[i];
            hash *= 16777619u;  /* FNV-1a Prime */
        }
    }
    
    return hash;
}

/* Hash-Funktion für Integer-Schlüssel */
uint32_t myrtx_hash_integer(const void* key, size_t key_size) {
    /* Einfache Implementierung für Integer, unabhängig von der Größe */
    int value = *(const int*)key;
    
    /* Knuth's Multiplikationsmethode */
    return (uint32_t)(value * 2654435761u);
}

/* Vergleichsfunktion für String-Schlüssel */
bool myrtx_compare_string_keys(const void* key1, size_t key1_size, 
                              const void* key2, size_t key2_size) {
    const char* str1 = (const char*)key1;
    const char* str2 = (const char*)key2;
    
    /* Bei Länge 0 die Strings als null-terminiert behandeln */
    if (key1_size == 0 && key2_size == 0) {
        return strcmp(str1, str2) == 0;
    } else if (key1_size == 0) {
        /* Erste String null-terminiert, zweiter mit bekannter Länge */
        size_t len1 = strlen(str1);
        if (len1 != key2_size - 1) {  /* -1 für den Null-Terminator */
            return false;
        }
        return memcmp(str1, str2, len1) == 0;
    } else if (key2_size == 0) {
        /* Zweiter String null-terminiert, erster mit bekannter Länge */
        size_t len2 = strlen(str2);
        if (key1_size - 1 != len2) {  /* -1 für den Null-Terminator */
            return false;
        }
        return memcmp(str1, str2, len2) == 0;
    } else {
        /* Beide Strings mit bekannter Länge */
        if (key1_size != key2_size) {
            return false;
        }
        return memcmp(str1, str2, key1_size) == 0;
    }
}

/* Vergleichsfunktion für Integer-Schlüssel */
bool myrtx_compare_integer_keys(const void* key1, size_t key1_size, 
                               const void* key2, size_t key2_size) {
    /* Einfacher Vergleich der Integer-Werte */
    return *(const int*)key1 == *(const int*)key2;
} 