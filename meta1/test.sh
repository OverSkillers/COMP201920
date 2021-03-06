# Clean previous tests
rm test/failed/*.out

# Build jucompiler if not already built
if ! [ -f jucompiler ]; then
    echo "jucompiler not built yet, running build script..."
    bash build.sh
fi

PASSED=0
FAILED=0
SKIPPED=0

BLUE=$(tput setaf 4)
GREEN=$(tput setaf 2)
RED=$(tput setaf 1)

PASSSTR="${GREEN}PASS"
FAILSTR="${RED}FAIL"
SKIPSTR="${BLUE}SKIP"

shopt -s nullglob

echo ""
echo "Starting test..."
echo ""


# Test every .juc or .java file
for filename in ./test/input/*.{juc,java}; do
    FILE=${filename##*/}

    padlimit=50
    pad=$(printf '%*s' "$padlimit")
    pad=${pad// /-}
    padlength=50

    # Run jucompiler with this test case
    if [ $FILE == "errors.java" ] || [ $FILE == "InsertionSort.java" ]; then
        ./jucompiler -e1 < $filename > output.txt
    else
        ./jucompiler -l < $filename > output.txt
    fi

    # Check if an .out file doesn't exist for this test
    if ! [ -f ./test/output/${FILE%.*}.out ]; then
	printf '%s%*.*s%s\n' "$FILE" 0 $(($padlength - ${#FILE})) "$pad" "$SKIPSTR"
        ((SKIPPED+=1))
	tput sgr0
	continue
    fi
	
    # Otherwise run a diff on them
    if [ "$(diff -y --suppress-common-lines ./test/output/${FILE%.*}.out output.txt | wc -l)" -ne "0" ]; then
        # If files are different, move it to failed directory
        cp output.txt test/failed/${FILE%.*}.out
	printf '%s%*.*s%s\n' "$FILE" 0 $((padlength - ${#FILE} )) "$pad" "$FAILSTR"
        ((FAILED+=1))
	tput sgr0
    else
	printf '%s%*.*s%s\n' "$FILE" 0 $((padlength - ${#FILE} )) "$pad" "$PASSSTR"
        ((PASSED+=1))
	tput sgr0
    fi
done

echo ""
echo ""

# Print final stats
echo "PASSED: $PASSED"
echo "FAILED: $FAILED"
echo "SKIPPED: $SKIPPED"

shopt -u nullglob

# Clean up
rm output.txt
