call ../cmake_android.bat ../Android_Build_armeabi_Release -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=armeabi -DURHO3D_LIB_TYPE=SHARED -DANDROID_STRIP=TRUE -DURHO3D_C++11=1 -DURHO3D_SAMPLES=0 -DURHO3D_TOOLS=FALSE -DURHO3D_NAVIGATION=FALSE -DURHO3D_NETWORK=FALSE -DURHO3D_LUA=FALSE -DURHO3D_ANGELSCRIPT=FALSE -DURHO3D_DATABASE_SQLITE=TRUE -DURHO3D_IK=0 -DURHO3D_WEBP=0 -DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.9 -DURHO3D_PHYSICS=TRUE -DANDROID_STL=gnustl_shared
cd ../Android_Build_armeabi_Release
REM 17 - Android SDK 18
android update project -t 14 -p . -s & make -j8
