#!/bin/sh

executable_dir=`dirname "${MW_OUTPUT_FILE}"`
Resources="${executable_dir}/../Resources"

# copy cvsgui tool

if [ -f "${Resources}/cvsgui" ]; then
	chflags nouchg "${Resources}/cvsgui"
	rm "${Resources}/cvsgui"
fi
cp "${MW_PROJECT_DIRECTORY}/../../cvs-1.10/build/cvsgui" "${Resources}/cvsgui"
if [ "$MW_CURRENT_TARGET" = "MacCvs_rel_machO" ]; then
  chmod +w "${Resources}"/cvsgui
  strip "${Resources}"/cvsgui
  if [ -f "${MW_OUTPUT_FILE}.xSYM" ]; then
    mv "${MW_OUTPUT_FILE}.xSYM" "${MW_OUTPUT_DIRECTORY}/"
  fi
  if [ -f "${MW_OUTPUT_FILE}.xMap" ]; then
    mv "${MW_OUTPUT_FILE}.xMap" "${MW_OUTPUT_DIRECTORY}/"
  fi
fi
chmod 755 "${Resources}"/cvsgui

# copy external diff helpers

if [ -f "${Resources}"/diff_in_codewarrior ]; then
	chflags nouchg "${Resources}"/diff_in_codewarrior
	rm "${Resources}"/diff_in_codewarrior
fi
cp "${MW_PROJECT_DIRECTORY}"/../rsrc/diff_in_codewarrior "${Resources}"/diff_in_codewarrior
chmod ugo+x "${Resources}"/diff_in_codewarrior

if [ -f "${Resources}/diff_in_bbedit" ]; then
	chflags nouchg "${Resources}/diff_in_bbedit"
	rm "${Resources}/diff_in_bbedit"
fi
cp "${MW_PROJECT_DIRECTORY}/../rsrc/diff_in_bbedit" "${Resources}/diff_in_bbedit"
chmod ugo+x "${Resources}"/diff_in_bbedit

if [ -f "${Resources}/diff_in_bbedit.scpt" ]; then
	chflags nouchg "${Resources}/diff_in_bbedit.scpt"
	rm "${Resources}/diff_in_bbedit.scpt"
fi
cp "${MW_PROJECT_DIRECTORY}/../rsrc/diff_in_bbedit.scpt" "${Resources}/diff_in_bbedit.scpt"

# copy the mfcshell.py script
if [ -f "${Resources}"/mfcshell.py ]; then
	chflags nouchg "${Resources}"/mfcshell.py
	rm "${Resources}"/mfcshell.py
fi
cp "${MW_PROJECT_DIRECTORY}"/../rsrc/mfcshell.py "${Resources}"/mfcshell.py
