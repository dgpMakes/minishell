echo "Zipping Authors.txt and msh.c in current directory"
make
rm -rf testdir
rm ssoo_p2_100405855_100405799_100405867.zip
zip ssoo_p2_100405855_100405799_100405867.zip msh.c Authors.txt
bash probador_ssoo_p2.sh ssoo_p2_100405855_100405799_100405867.zip