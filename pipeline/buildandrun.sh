SOURCE="pipeline.cpp"
BINARY_BASE="pipeline"
 
if [ ! -f "$SOURCE" ]; then
    echo "Error: $SOURCE not found in current directory."
    exit 1
fi
 
for LEVEL in 0 1 2 3; do
    OPT="-O${LEVEL}"
    BINARY="${BINARY_BASE}_O${LEVEL}"
 
    echo "========================================"
    echo "Compiling with ${OPT}..."
    echo "========================================"
 
    if clang++ -std=c++17 "$OPT" -o "$BINARY" "$SOURCE"; then
        echo "Compiled successfully -> $BINARY"
        echo "Running $BINARY..."
        echo "----------------------------------------"
        ./"$BINARY"
        echo "----------------------------------------"
        echo "Exit code: $?"
    else
        echo "Compilation failed at ${OPT}."
    fi
 
    echo ""
done
