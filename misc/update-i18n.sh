#!/bin/sh

LUPDATE="lupdate"
LRELEASE="lrelease"

if lupdate-qt4 -version >/dev/null 2>&1; then
    LUPDATE="lupdate-qt4"
    LRELEASE="lrelease-qt4"
fi

LANGUAGES="ru en_GB en_US cs_CZ"

for LANG in $LANGUAGES; do
    $LUPDATE \
	../svcore/*/*.h ../svcore/*/*.cpp \
	../svcore/*/*/*.h ../svcore/*/*/*.cpp \
	../svgui/*/*.h ../svgui/*/*.cpp \
	../svapp/*/*.h ../svapp/*/*.cpp \
	*/*.h */*.cpp \
	-ts i18n/sonic-visualiser_$LANG.ts
done

for LANG in $LANGUAGES; do
    $LRELEASE i18n/sonic-visualiser_$LANG.ts
done

