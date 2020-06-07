mkdir deploy
cd deploy
cmake ..
sudo make install
cd ..
cp deploy/install_manifest.txt install_manifest.txt
sudo rm -r deploy
echo Libreria compartida -Delibird- instalada!