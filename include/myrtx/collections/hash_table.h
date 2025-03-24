/**
 * @file hash_table.h
 * @brief Hash-Tabellen-Implementierung für die myrtx-Bibliothek
 * 
 * Diese Datei enthält die Definitionen und Funktionen für eine generische 
 * Hash-Tabelle, die verschiedene Schlüssel- und Werttypen unterstützt und 
 * sowohl mit Arena-Allokation als auch mit standard malloc/free arbeiten kann.
 */

#ifndef MYRTX_HASH_TABLE_H
#define MYRTX_HASH_TABLE_H

#include "myrtx/memory/arena_allocator.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaker Typ für eine Hash-Tabelle
 */
typedef struct myrtx_hash_table_t myrtx_hash_table_t;

/**
 * @brief Hash-Funktion
 * 
 * @param key Zeiger auf den Schlüssel
 * @param key_size Größe des Schlüssels in Bytes
 * @return Berechneter Hash-Wert
 */
typedef uint32_t (*myrtx_hash_function)(const void* key, size_t key_size);

/**
 * @brief Schlüsselvergleichsfunktion
 * 
 * @param key1 Zeiger auf den ersten Schlüssel
 * @param key1_size Größe des ersten Schlüssels in Bytes
 * @param key2 Zeiger auf den zweiten Schlüssel
 * @param key2_size Größe des zweiten Schlüssels in Bytes
 * @return true wenn die Schlüssel gleich sind, sonst false
 */
typedef bool (*myrtx_key_compare_function)(const void* key1, size_t key1_size, 
                                          const void* key2, size_t key2_size);

/**
 * @brief Erstellt eine neue Hash-Tabelle
 * 
 * @param arena Optionaler Arena-Allokator (NULL für malloc/free)
 * @param initial_capacity Anfangskapazität (0 für Standardwert)
 * @param hash_function Hash-Funktion für Schlüssel
 * @param compare_function Vergleichsfunktion für Schlüssel
 * @return Zeiger auf die neue Hash-Tabelle oder NULL bei Fehler
 */
myrtx_hash_table_t* myrtx_hash_table_create(myrtx_arena_t* arena, 
                                           size_t initial_capacity,
                                           myrtx_hash_function hash_function, 
                                           myrtx_key_compare_function compare_function);

/**
 * @brief Gibt eine Hash-Tabelle frei
 * 
 * @param table Zeiger auf die Hash-Tabelle
 * @param free_keys Ob die Schlüssel freigegeben werden sollen
 * @param free_values Ob die Werte freigegeben werden sollen
 */
void myrtx_hash_table_free(myrtx_hash_table_t* table, 
                          bool free_keys, 
                          bool free_values);

/**
 * @brief Fügt einen Schlüssel-Wert-Paar zur Hash-Tabelle hinzu oder aktualisiert es
 * 
 * @param table Zeiger auf die Hash-Tabelle
 * @param key Zeiger auf den Schlüssel
 * @param key_size Größe des Schlüssels (0 für Strings bedeutet Nutzung von strlen)
 * @param value Zeiger auf den Wert
 * @param value_size Größe des Werts
 * @return true bei Erfolg, false bei Fehler
 */
bool myrtx_hash_table_put(myrtx_hash_table_t* table, 
                         const void* key, 
                         size_t key_size,
                         const void* value, 
                         size_t value_size);

/**
 * @brief Holt einen Wert aus der Hash-Tabelle
 * 
 * @param table Zeiger auf die Hash-Tabelle
 * @param key Zeiger auf den Schlüssel
 * @param key_size Größe des Schlüssels (0 für Strings bedeutet Nutzung von strlen)
 * @param[out] value_out Zeiger auf Speicherort für den gefundenen Wert
 * @param[out] value_size_out Zeiger auf Speicherort für die Größe des Werts
 * @return true wenn der Schlüssel gefunden wurde, sonst false
 */
bool myrtx_hash_table_get(const myrtx_hash_table_t* table, 
                         const void* key, 
                         size_t key_size,
                         void** value_out, 
                         size_t* value_size_out);

/**
 * @brief Prüft, ob ein Schlüssel in der Hash-Tabelle existiert
 * 
 * @param table Zeiger auf die Hash-Tabelle
 * @param key Zeiger auf den Schlüssel
 * @param key_size Größe des Schlüssels (0 für Strings bedeutet Nutzung von strlen)
 * @return true wenn der Schlüssel gefunden wurde, sonst false
 */
bool myrtx_hash_table_contains_key(const myrtx_hash_table_t* table, 
                                  const void* key, 
                                  size_t key_size);

/**
 * @brief Entfernt einen Eintrag aus der Hash-Tabelle
 * 
 * @param table Zeiger auf die Hash-Tabelle
 * @param key Zeiger auf den Schlüssel
 * @param key_size Größe des Schlüssels (0 für Strings bedeutet Nutzung von strlen)
 * @param free_key Ob der Schlüssel freigegeben werden soll
 * @param free_value Ob der Wert freigegeben werden soll
 * @return true wenn der Schlüssel gefunden und entfernt wurde, sonst false
 */
bool myrtx_hash_table_remove(myrtx_hash_table_t* table, 
                            const void* key, 
                            size_t key_size,
                            bool free_key, 
                            bool free_value);

/**
 * @brief Gibt die Anzahl der Einträge in der Hash-Tabelle zurück
 * 
 * @param table Zeiger auf die Hash-Tabelle
 * @return Anzahl der Einträge
 */
size_t myrtx_hash_table_size(const myrtx_hash_table_t* table);

/**
 * @brief Leert die Hash-Tabelle, entfernt alle Einträge
 * 
 * @param table Zeiger auf die Hash-Tabelle
 * @param free_keys Ob die Schlüssel freigegeben werden sollen
 * @param free_values Ob die Werte freigegeben werden sollen
 */
void myrtx_hash_table_clear(myrtx_hash_table_t* table, 
                           bool free_keys, 
                           bool free_values);

/**
 * @brief Standard-Hash-Funktion für Strings
 * 
 * @param key Zeiger auf den String-Schlüssel
 * @param key_size Größe des Schlüssels (0 bedeutet Nutzung von strlen)
 * @return Hash-Wert
 */
uint32_t myrtx_hash_string(const void* key, size_t key_size);

/**
 * @brief Standard-Hash-Funktion für Integer-Schlüssel
 * 
 * @param key Zeiger auf den Integer-Schlüssel
 * @param key_size Größe des Schlüssels (ignoriert, muss sizeof(int) sein)
 * @return Hash-Wert
 */
uint32_t myrtx_hash_integer(const void* key, size_t key_size);

/**
 * @brief Standard-Vergleichsfunktion für String-Schlüssel
 * 
 * @param key1 Zeiger auf den ersten String-Schlüssel
 * @param key1_size Größe des ersten Schlüssels (0 bedeutet Nutzung von strlen)
 * @param key2 Zeiger auf den zweiten String-Schlüssel
 * @param key2_size Größe des zweiten Schlüssels (0 bedeutet Nutzung von strlen)
 * @return true wenn die Strings gleich sind, sonst false
 */
bool myrtx_compare_string_keys(const void* key1, size_t key1_size, 
                              const void* key2, size_t key2_size);

/**
 * @brief Standard-Vergleichsfunktion für Integer-Schlüssel
 * 
 * @param key1 Zeiger auf den ersten Integer-Schlüssel
 * @param key1_size Größe des ersten Schlüssels (ignoriert, muss sizeof(int) sein)
 * @param key2 Zeiger auf den zweiten Integer-Schlüssel
 * @param key2_size Größe des zweiten Schlüssels (ignoriert, muss sizeof(int) sein)
 * @return true wenn die Integer gleich sind, sonst false
 */
bool myrtx_compare_integer_keys(const void* key1, size_t key1_size, 
                               const void* key2, size_t key2_size);

#ifdef __cplusplus
}
#endif

#endif /* MYRTX_HASH_TABLE_H */ 