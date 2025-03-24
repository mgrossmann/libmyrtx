#!/bin/bash
# build.sh - Build-Skript für das libmyrtx-Projekt
# 
# Dieses Skript automatisiert den Build-Prozess für die libmyrtx-Bibliothek
# mit verschiedenen Konfigurationsoptionen.

set -e # Bei Fehlern abbrechen

BUILD_TYPE="Debug"
RUN_TESTS=0
BUILD_EXAMPLES=1
CLEAN=0
INSTALL=0
INSTALL_PREFIX="/usr/local"
BUILD_DIR="build"

# Hilfe-Funktion
show_help() {
    echo "Verwendung: $0 [Optionen]"
    echo
    echo "Optionen:"
    echo "  -h, --help               Zeigt diese Hilfe an"
    echo "  -t, --type TYPE          Setzt den Build-Typ (Debug oder Release) [Standard: Debug]"
    echo "  -c, --clean              Bereinigt das Build-Verzeichnis vor dem Bauen"
    echo "  --test                   Führt Tests nach dem Bauen aus"
    echo "  --no-examples            Baut keine Beispiele"
    echo "  -i, --install            Installiert die Bibliothek nach dem Bauen"
    echo "  --prefix PATH            Setzt den Installationspfad [Standard: /usr/local]"
    echo "  --build-dir DIR          Setzt das Build-Verzeichnis [Standard: build]"
    echo
}

# Parameter verarbeiten
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        --test)
            RUN_TESTS=1
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES=0
            shift
            ;;
        -i|--install)
            INSTALL=1
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        *)
            echo "Unbekannte Option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Build-Typ überprüfen
if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
    echo "Ungültiger Build-Typ: $BUILD_TYPE. Gültige Werte sind 'Debug' oder 'Release'."
    exit 1
fi

# Build-Verzeichnis erstellen, wenn es noch nicht existiert
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

# Build-Verzeichnis bereinigen, wenn angefordert
if [ $CLEAN -eq 1 ]; then
    echo "Bereinige Build-Verzeichnis..."
    rm -rf "$BUILD_DIR"/*
fi

# Ins Build-Verzeichnis wechseln
cd "$BUILD_DIR"

# CMake-Konfiguration
echo "Konfiguriere CMake für $BUILD_TYPE-Build..."
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ $BUILD_EXAMPLES -eq 0 ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DMYRTX_BUILD_EXAMPLES=OFF"
fi

# Setze den Installationspfad, wenn die Installation aktiviert ist
if [ $INSTALL -eq 1 ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
fi

cmake $CMAKE_ARGS ..

# Build durchführen
echo "Baue libmyrtx..."
cmake --build . -- -j$(nproc)

# Tests ausführen, wenn angefordert
if [ $RUN_TESTS -eq 1 ]; then
    echo "Führe Tests aus..."
    ctest --output-on-failure
fi

# Installieren, wenn angefordert
if [ $INSTALL -eq 1 ]; then
    echo "Installiere libmyrtx in $INSTALL_PREFIX..."
    cmake --install .
fi

echo "Build abgeschlossen!"

if [ $BUILD_EXAMPLES -eq 1 ]; then
    echo "Beispielprogramme befinden sich in $BUILD_DIR/examples/"
fi

cd .. 