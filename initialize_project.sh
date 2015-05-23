#!/bin/sh

if [ "$#" -ne 2 ]; then
  echo "Usage: ./initialize_project.sh applicationName /path/to/openframeworks/root/folder"
  exit
fi

APP_NAME=$1
PATH_TO_ROOT="../$2"

# check that we are executing the scriptr from the directory where the .sh file is
if  [ ! -f "./initialize_project.sh" ];
then
   echo "\n./initialize_project.sh must be called from its directory\n"
   exit
fi

# check that the path to OF exists
if [ ! -d $2 ];
then
   echo "\nThe path $2 doesn't exist or is not a directory. Insert a valid path to the openframeworks root"
   exit
fi

echo ""

echo "Initializing project named $1 in $(pwd)..."
echo "Executing operations for the Android platform"

OP_TITLE="\tChanging the app_name key in the file ./project_android/res/values/strings.xml to $APP_NAME"
sed -i '' -E 's/(<string name="app_name">)[^>]+(<\/string>)/\1'$1'\2/g' ./project_android/res/values/strings.xml
rc=$?;
if [[ $rc != 0 ]]; then echo "$OP_TITLE \033[31mFAIL\033[0m" exit $rc; else  echo "$OP_TITLE \033[32mOK\033[0m"; fi

OP_TITLE="\tCreating the file rootPath.configureMe in the current directory containing the path to the OF directory"
cat <<- EOF > rootPath.configureMe
OF_PATH = $PATH_TO_ROOT
EOF
rc=$?;
if [[ $rc != 0 ]]; then echo "$OP_TITLE \033[31mFAIL\033[0m" exit $rc; else  echo "$OP_TITLE \033[32mOK\033[0m"; fi

OP_TITLE="\tChanging the reference to the ofxAndroid project in key in project.properties"
sed -i '' -E "s@android.library.reference.1=.*@android.library.reference.1=$PATH_TO_ROOT/addons/ofxAndroid/ofAndroidLib@g" ./project_android/project.properties
rc=$?;
if [[ $rc != 0 ]]; then echo "$OP_TITLE \033[31mFAIL\033[0m" exit $rc; else  echo "$OP_TITLE \033[32mOK\033[0m"; fi

OP_TITLE="\tChanging the name of the app in android Manifest"
sed -i '' -E "s@package=.*@package=\"com.dinahmoe.$APP_NAME\"@g" ./project_android/AndroidManifest.xml
rc=$?;
if [[ $rc != 0 ]]; then echo "$OP_TITLE \033[31mFAIL\033[0m" exit $rc; else  echo "$OP_TITLE \033[32mOK\033[0m"; fi

OP_TITLE="\tChanging the name of the app in config.make, used for building the resources zip file"
sed -i '' -E "s@APPNAME = .*@APPNAME = $APP_NAME@g" ./project_android/config.make
rc=$?;
if [[ $rc != 0 ]]; then echo "$OP_TITLE \033[31mFAIL\033[0m" exit $rc; else  echo "$OP_TITLE \033[32mOK\033[0m"; fi


OP_TITLE="\tChanging the package name of the app in config.make, used for installing and running"
sed -i '' -E "s@PKGNAME = .*@PKGNAME = com.dinahmoe.$APP_NAME@g" ./project_android/config.make
rc=$?;
if [[ $rc != 0 ]]; then echo "$OP_TITLE \033[31mFAIL\033[0m" exit $rc; else  echo "$OP_TITLE \033[32mOK\033[0m"; fi


OP_TITLE="\tChanging the package declaration in the Java source file"
sed -i '' -E "s@package .*@package com.dinahmoe.$APP_NAME;@g" ./project_android/srcJava/com/dinahmoe/ofDinahmoeApp/OFActivity.java
rc=$?;
if [[ $rc != 0 ]]; then echo "$OP_TITLE \033[31mFAIL\033[0m" exit $rc; else  echo "$OP_TITLE \033[32mOK\033[0m"; fi

echo ""

echo "For the other platforms must be man made, I am going crazy with sed"

echo ""