# Start building commands
LEX="lex jucompiler.l"
YACC="yacc -d jucompiler.y"
CLANG="clang-3.9 -Wall -Wno-unused-function -o jucompiler lex.yy.c y.tab.c ast.c -lm"

# Check for any arguments
while getopts "vo" OPTION; do
    case $OPTION in
    v)
        CLANG=$CLANG" -v -DDEBUG=1"
        LEX="lex -v jucompiler.l"
        YACC="yacc -d --debug --verbose jucompiler.y"
        ;;
    o)
        YACC="yacc -d -v jucompiler.y"
    esac
done

# Compile lex file, checking for build success
if $LEX; then

    # Compile yacc file, checking for build success
    if $YACC; then

        # Compile using clang, checking for build success
        if $CLANG; then
            # Zip the lex file if build successful
            zip jucompiler.zip jucompiler.l jucompiler.y ast.c ast.h structures.h
        else
            exit
        fi

    else
        exit
    fi

else
    exit
fi
