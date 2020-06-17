mkdir deploy
cd deploy
cmake ..
sudo make install
cd ..
cp deploy/install_manifest.txt install_manifest.txt
sudo rm -r deploy
if [ $? -eq 0 ]
then
  echo "$(tput setaf 2)Libreria compartida -Delibird- instalada!$(tput sgr0)"
else
  echo "$(tput setaf 1)Error al instalar libreria compartida$(tput sgr0)"
fi