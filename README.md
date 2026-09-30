# Kutuphane-Yonetim-Sistemi

Bu proje, C programlama dili kullanılarak geliştirilmiş, konsol tabanlı bir kütüphane yönetim uygulamasıdır. Projede veri saklama işlemleri için yerel dosyalar (File I/O) kullanılmış ve Struct yapıları üzerinden bellek yönetimi sağlanmıştır.

Özellikler
Sistemde yetki düzeylerine göre üç farklı kullanıcı rolü bulunmaktadır: Super Admin, Yönetici ve Kullanıcı.

Admin ve Yönetici İşlemleri:

Yeni kullanıcı ve yönetici hesapları oluşturma.

Sisteme benzersiz ID ile yeni kitap ekleme, mevcut kitapları silme veya bilgilerini güncelleme.

Tüm kitapları ve sistemde anlık olarak ödünç verilmiş kitapları listeleme.

15 günlük teslim süresi dolmuş kitapları ve kullanıcıları tespit etme.

Kullanıcı İşlemleri:

ID, Kitap Adı, Yazar Adı veya Kategoriye göre detaylı kitap arama.

Rafta bulunan kitapları listeleme.

Maksimum 3 adet sınırıyla kitap ödünç alma.

Ödünç alınan kitapların iade tarihini görüntüleme ve iade işlemini gerçekleştirme.

Kurulum ve Çalıştırma
Projeyi kendi bilgisayarınızda derleyip çalıştırmak için sisteminizde bir C derleyicisi (örneğin GCC) kurulu olmalıdır.

Repoyu bilgisayarınıza klonlayın:
git clone https://github.com/MelihCan45/Kutuphane-Yonetim-Sistemi.git

cd Kutuphane-Yonetim-Sistemi.git

gcc MainCode.c -o kutuphane

Programı Çalıştırın:
1-Windows için; kutuphane.exe
2-Linux/MacOS için terminalde; ./kutuphane

Not: Sistemi ilk kez çalıştırdığınızda kullanıcı hesaplarını yönetmek için varsayılan Super Admin bilgileri ile giriş yapmanız gerekmektedir.

Kullanıcı Adı: admin

Şifre: admin123
