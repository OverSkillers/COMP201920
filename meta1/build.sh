# Start building commands
LEX="lex jucompiler.l"
CLANG="clang-3.9 -Wall lex.yy.c -o jucompiler"

# Check for any arguments
while getopts "v" OPTION; do
    case $OPTION in
    v)
        CLANG=$CLANG" -v"
        LEX="lex -v jucompiler.l"
    esac
done

# Compile lex file, checking for build success
if $LEX; then

    # Compile using clang, checking for build success
    if $CLANG; then

        # Zip the lex file if build successful
        zip jucompiler.zip jucompiler.l

    else

        exit

    fi

else

    exit

fi