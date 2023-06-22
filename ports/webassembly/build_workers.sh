#!/bin/bash

ROOT=$PWD/node_modules/monaco-editor/esm/vs
OPTS="-d parcel_bundle_out --no-source-maps --log-level 1"        # Parcel options - See: https://parceljs.org/cli.html
parcel build $ROOT/editor/editor.worker.js $OPTS
mkdir -p bundle_out
mv parcel_bundle_out/* bundle_out
rm -rf parcel_bundle_out
