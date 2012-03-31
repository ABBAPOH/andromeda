function update_lib {
    filename=$(basename $1)
    echo "updating" $1/translations/${filename}_$2.ts
    lupdate $1/ -ts $1/translations/${filename}_$2.ts $3
}

function update_plugin {
    filename=$(basename $1)
    echo "updating" $1/translations/${filename}plugin_$2.ts
    lupdate $1/ -ts $1/translations/${filename}plugin_$2.ts $3
}

function main {
    for lib in src/libs/* ; do
        update_lib $lib $1 $2
    done
    for plugin in src/plugins/* ; do
        update_plugin $plugin $1 $2
    done
}

if [ -z "$1" ]; then
        echo "usage ./lupdate.sh <language>"
        exit
fi

main $1 $2
