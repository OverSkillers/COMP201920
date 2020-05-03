# Start building commands
LEX="lex jucompiler.l"
YACC="yacc -d jucompiler.y"
CLANG="clang-3.9 -Wall -Wno-unused-function -o jucompiler lex.yy.c y.tab.c ast.c semantics.c symbol.c -lm"
UPLOAD=0

# Check for any arguments
while getopts "vpou" OPTION; do
    case $OPTION in
    v)
        CLANG=$CLANG" -g -v -DDEBUG=1"
        LEX="lex -v jucompiler.l"
        YACC="yacc -d --debug --verbose jucompiler.y"
        ;;
    p)
        CLANG=$CLANG" -g -v -DDEBUG=2"
        ;;
    o)
        YACC="yacc -d -v jucompiler.y"
        ;;
    u)
        UPLOAD=1
    esac
done

# Compile lex file, checking for build success
if $LEX; then

    # Compile yacc file, checking for build success
    if $YACC; then

        # Compile using clang, checking for build success
        if $CLANG; then
            # Zip the lex file if build successful
            zip jucompiler.zip jucompiler.* ast.* symbol.* semantics.*

            # Upload the file to transfer.sh
            if [[ "$UPLOAD" -eq "1" ]]; then
                curl --upload-file ./jucompiler.zip https://transfer.sh/jucompiler.zip
                echo ""
            fi
        else
            exit
        fi

    else
        exit
    fi

else
    exit
fi
