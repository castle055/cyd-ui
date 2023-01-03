#!/usr/bin/env sh

cd "$(dirname "$0")" || exit 1

echo "[INSTALL] Copying library executable to /usr/lib..."
sudo cp -v ../cmake-build-release/libcyd_ui.so /usr/lib/
echo "[INSTALL] Making file executable..."
sudo chmod +x /usr/lib/libcyd_ui.so
echo "[INSTALL] Copying library headers to /usr/include..."
sudo mkdir /usr/include/cyd_ui
sudo cp -rv ../include/*.hpp /usr/include/cyd_ui
sudo cp -rv ../include/*.h /usr/include/cyd_ui
echo "[INSTALL] Done"
#sudo chown root root /usr/lib/libcyd_ui.so
