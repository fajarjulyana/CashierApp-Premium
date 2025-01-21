# Membuat struktur direktori
mkdir -p CashierApp/DEBIAN
mkdir -p CashierApp/usr/bin
mkdir -p CashierApp/usr/share/icons/hicolor/256x256/apps
mkdir -p CashierApp/usr/share/applications

# Membuat file control
cat > CashierApp/DEBIAN/control << EOF
Package: cashierapp
Version: 1.0
Section: utils
Priority: optional
Architecture: amd64
Depends: libc6 (>= 2.7), libgtk-3-0 (>= 3.0), libgdk-pixbuf2.0-0 (>= 2.30)
Maintainer: Nama Anda <email@domain.com>
Description: Aplikasi kasir untuk manajemen transaksi penjualan dengan management input dan management admin
EOF

# Menyalin aplikasi CashierApp (sesuaikan dengan path aplikasi Anda)
# Misalnya aplikasi CashierApp berada di /path/to/CashierApp
cp ../CashierApp CashierApp/usr/bin/
cp ../ManageApp CashierApp/usr/bin/
cp ../AdminApp CashierApp/usr/bin/
# Menyalin ikon (sesuaikan dengan lokasi file ikon)
cp ../cashierapp.png CashierApp/usr/share/icons/hicolor/256x256/apps/
cp ../adminapp.png CashierApp/usr/share/icons/hicolor/256x256/apps/
cp ../manageapp.png CashierApp/usr/share/icons/hicolor/256x256/apps/
# Menambahkan file desktop (opsional)
cat > CashierApp/usr/share/applications/cashierapp.desktop << EOF
[Desktop Entry]
Name=CashierApp
Comment=Aplikasi kasir untuk manajemen transaksi penjualan
Exec=/usr/bin/CashierApp
Icon=cashierapp
Terminal=false
Type=Application
Categories=Utility;Application;
EOF

cat > CashierApp/usr/share/applications/manageapp.desktop << EOF
[Desktop Entry]
Name=ManageApp
Comment=Aplikasi management untuk manajemen transaksi penjualan
Exec=/usr/bin/ManageApp
Icon=manageapp
Terminal=false
Type=Application
Categories=Utility;Application;
EOF

cat > CashierApp/usr/share/applications/adminapp.desktop << EOF
[Desktop Entry]
Name=AdminApp
Comment=Aplikasi admin untuk manajemen transaksi penjualan
Exec=/usr/bin/AdminApp
Icon=adminapp
Terminal=false
Type=Application
Categories=Utility;Application;
EOF
# Membuat paket .deb
dpkg-deb --build CashierApp
