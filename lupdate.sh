function update {
    filename=$(basename $1)
    echo "updating" $1/translations/${filename}_$2.ts
    lupdate $1/ -ts $1/translations/${filename}_$2.ts $3
}

function main {
    for lib in libs/src/* ; do
        update $lib $1 $2
    done
    for plugin in src/plugins/parts/* ; do
        update $plugin $1 $2
    done

    update src/app $1 $2
}

if [ -z "$1" ]; then
        echo "usage ./lupdate.sh <language>"
        exit
fi

main $1 $2
