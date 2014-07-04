#!/bin/bash

# Execute this from the top-level directory of the project (the one
# that contains the .app bundle).  Supply the name of the .app bundle
# as argument (the target will use $app.app regardless, but we need
# to know the source)

source="$1"
dmg="$2"
if [ -z "$source" ] || [ ! -d "$source" ] || [ -z "$dmg" ]; then
	echo "Usage: $0 <source.app> <target-dmg-basename>"
	echo "  e.g. $0 MyApplication.app MyApplication"
 	echo "  Version number and .dmg will be appended automatically,"
        echo "  but the .app name must include .app"
	exit 2
fi
app=`basename "$source" .app`

version=`perl -p -e 's/^[^"]*"([^"]*)".*$/$1/' version.h`
case "$version" in
    [0-9].[0-9]) bundleVersion="$version".0 ;;
    [0-9].[0-9].[0-9]) bundleVersion="$version" ;;
    *) echo "Error: Version $version is neither two- nor three-part number" ;;
esac

echo
echo "Fixing up paths."

deploy/osx/paths.sh "$app"

echo
echo "Making target tree."

volume="$app"-"$version"
target="$volume"/"$app".app
dmg="$dmg"-"$version".dmg

mkdir "$volume" || exit 1

ln -s /Applications "$volume"/Applications
cp README README.OSC COPYING CHANGELOG "$volume/"
cp -rp "$source" "$target"

echo "Done"

echo
echo "Copying in qt.conf to set local-only plugin paths."
echo "Make sure all necessary Qt plugins are in $target/Contents/plugins/*"
echo "You probably want platforms/, accessible/ and imageformats/ subdirectories."
cp deploy/osx/qt.conf "$target"/Contents/Resources/qt.conf

echo
echo "Writing version $bundleVersion in to bundle."
echo "(This should be a three-part number: major.minor.point)"

perl -p -e "s/SV_VERSION/$bundleVersion/" deploy/osx/Info.plist \
    > "$target"/Contents/Info.plist

echo "Done: check $target/Contents/Info.plist for sanity please"

deploy/osx/sign.sh "$volume" || exit 1

echo
echo "Making dmg..."

hdiutil create -srcfolder "$volume" "$dmg" -volname "$volume" && 
	rm -r "$volume"

echo "Done"
