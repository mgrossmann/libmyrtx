#!/bin/bash
set -e

# Erstelle fehlende Verzeichnisse
mkdir -p source/_static source/_templates build

# Prüfe, ob die virtuelle Umgebung existiert
if [ ! -d "../.venv" ]; then
    echo "Virtuelle Python-Umgebung wird erstellt..."
    python3 -m venv ../.venv
    source ../.venv/bin/activate
    pip install -r requirements.txt
else
    source ../.venv/bin/activate
fi

# Erstelle Doxygen-Dokumentation für API-Referenz
if [ -f "Doxyfile" ]; then
    echo "Generiere Doxygen XML..."
    doxygen Doxyfile
fi

# Erstelle HTML-Dokumentation
echo "Erstelle HTML-Dokumentation..."
python -m sphinx -b html source build/html

echo "Dokumentation wurde erstellt in: build/html/index.html" 