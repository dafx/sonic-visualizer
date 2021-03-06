#!/bin/bash

app="$1"
if [ -z "$app" ]; then
	echo "Usage: $0 <appname>"
	echo "Provide appname without the .app extension, please"
	exit 2
fi

frameworks="QtCore QtNetwork QtGui QtXml QtWidgets QtPrintSupport"

echo
echo "I expect you to have already copied these frameworks from the Qt installation to"
echo "$app.app/Contents/Frameworks -- expect errors to follow if they're missing:"
echo "$frameworks"
echo

echo "Fixing up loader paths in binaries..."

for fwk in $frameworks; do
    install_name_tool -id $fwk "$app.app/Contents/Frameworks/$fwk"
done

find "$app.app" -name \*.dylib -print | while read x; do
    install_name_tool -id "`basename \"$x\"`" "$x"
done

for fwk in $frameworks; do
        find "$app.app" -type f -print | while read x; do
                current=$(otool -L "$x" | grep "$fwk" | grep amework | awk '{ print $1; }')
                [ -z "$current" ] && continue
                echo "$x has $current"
                relative=$(echo "$x" | sed -e "s,$app.app/Contents/,," \
                        -e 's,[^/]*/,../,g' -e 's,/[^/]*$,/Frameworks/'"$fwk"',' )
                echo "replacing with relative path $relative"
                install_name_tool -change "$current" "@loader_path/$relative" "$x"
        done
done

echo "Done: be sure to run the app and see that it works!"


