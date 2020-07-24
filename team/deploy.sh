mkdir -p deploy
cd deploy
cmake ..
make
if [ $? -eq 0 ]
then
  echo "$(tput setaf 2)Proceso Team deployado!$(tput sgr0)"
else
  echo "$(tput setaf 1)Error al deployar$(tput sgr0)"
fi