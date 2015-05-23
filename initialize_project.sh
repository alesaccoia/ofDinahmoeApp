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


echo "\033[32mInitializing project named $1 in $(pwd)...\033[0m"
echo "\034[32mExecuting operations for the Android platform\033[0m"

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



echo "The rest of the stuff must be man made, I am going crazy with sed"