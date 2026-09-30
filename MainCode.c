#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#ifndef _WIN32
#include <unistd.h>
#endif
char aktifRol[10];
char adminAd[] = "admin", adminsifre[] = "admin123", adminRol[] = "Admin";//Global tanımlanmış bir süper admin.
int randNum = 0;//Benzersiz ID tanımlamak için rastgele sayı değişkeni
typedef struct {
    char KullaniciAdi[20];
    int KitapID;
    time_t alisTarihi;
} oduncKitaplar;
typedef struct {
    int ID;
    char kitapAdi[40];
    char Kategori[20];
    char Yazar[40];
    char kDurum[8];
} Kitaplar;
typedef struct {
    char kullaniciAdi[20];
    char sifre[20];
    char rol[10];
} Kullanicilar;
void dosyaSifirla(const char *dosyaAdi) {
    FILE *dosya = fopen(dosyaAdi, "wb");  // wb yazma modunda açar ve içeriği sıfırlar
    if (dosya == NULL) {
        printf("Dosya sifirlama hatasi! (%s)\n", dosyaAdi);
        fclose(dosya);
        return;
    }
    fclose(dosya);
    printf("%s dosyasi sifirlandi.\n", dosyaAdi);
}
void clearScreen(){
    #ifdef _WIN32
        system("cls");//Windows sistemler için.
    #else
        printf("\033[H\033[J");//Linux ve macOS işletim sistemleri için ANSI escape kodu. 
    #endif
}
void kategoriKontrol(char *Kategori){//Istenilen kategori dışında kategori girilirse tekrar girdi alır.
        while (strcmp(Kategori, "roman") != 0 && strcmp(Kategori, "egitici") != 0 &&
           strcmp(Kategori, "tarih") != 0 && strcmp(Kategori, "gezi") != 0 &&
           strcmp(Kategori, "fantastik") != 0) {
        printf("Gecersiz kategori! Tekrar giriniz(roman/tarih/egitici/gezi/fantastik):\n");
        fgets(Kategori, 20 , stdin);
        Kategori[strcspn(Kategori, "\n")] = 0;
    }
}
int isBenzersiz(int* sayilar, int* sayac, int randNum) {//Verilen dizi içinde randNum değişkeninin değeri varmı diye kontrol eder
    for (int i = 0; i < *sayac; i++) {
        if (sayilar[i] == randNum) return 0;
    }
    return 1;
}

int kullaniciEkle(const char* yeniKullaniciAdi, const char* yenisifre) {//Yoneticiden alınan kullanici adi ve şifreyle yeni kullanıcı oluşturur.
    FILE* kullanicilarDosya = fopen("kullanicilar1.bin", "ab+");
    if (kullanicilarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        return 0;
    }
    
    Kullanicilar NKayit;
    while(fread(&NKayit, sizeof(Kullanicilar),1,kullanicilarDosya)){
        if(strcmp(NKayit.kullaniciAdi,yeniKullaniciAdi)==0){
            printf("Kullanici adi daha once alinmistir!!\nYeniden Deneyiniz..\n");
            fclose(kullanicilarDosya);
            return 0;
        }
    }
    strcpy(NKayit.kullaniciAdi, yeniKullaniciAdi);
    strcpy(NKayit.sifre, yenisifre);
    strcpy(NKayit.rol, "Kullanici");
    fwrite(&NKayit, sizeof(Kullanicilar), 1, kullanicilarDosya);
    fclose(kullanicilarDosya);
    printf("Kullanici Basariyla Eklendi!\n");
    return 1;
}

int GirisDogrula(char* kullaniciAdi, char* sifre) {//Giriş yapmak için girilen kullanici adi ve şifrenin kullanicilar dosyasında var olup olmadıgını kontrol eder.
    if (strcmp(kullaniciAdi, adminAd) == 0 && strcmp(sifre, adminsifre) == 0) {//Başlangıcta tanımlanan admin bilgilerine eşitse girilen şifre ve kullanici adi admin girişi yapılır.
        strcpy(aktifRol, adminRol);
        printf("SuperAdmin olarak giris yapildi!\n");
        return 1;
    }
    FILE* kullanicilarDosya = fopen("kullanicilar1.bin", "rb");
    if (kullanicilarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        return 0;
    }
    Kullanicilar temp;
    
    while (fread(&temp, sizeof(Kullanicilar), 1, kullanicilarDosya)) {
        if (strcmp(kullaniciAdi, temp.kullaniciAdi) == 0 && strcmp(sifre, temp.sifre) == 0) {
            strcpy(aktifRol, temp.rol);
            fclose(kullanicilarDosya);
            printf("Giris basarili!\n");
            return 1;
        }
    }
    fclose(kullanicilarDosya);
    printf("Giris basarisiz!\n");
    return 0;
}

int yoneticiEkle(const char* yeniYoneticiAdi, const char* yenisifre) {//Admin tarafından girilen kullanici adi ve şifreyle yönetici ekler.
    FILE* kullanicilarDosya = fopen("kullanicilar1.bin", "ab+");
    if (kullanicilarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        return 0;
    }
    Kullanicilar NYonetici;
    while(fread(&NYonetici, sizeof(Kullanicilar),1,kullanicilarDosya)){
        if(strcmp(NYonetici.kullaniciAdi,yeniYoneticiAdi) == 0){
            printf("Kullanici adi daha once alinmis!\nYeniden deneyiniz!\n");
            fclose(kullanicilarDosya);
            return 0;
        }
    }
    
    strcpy(NYonetici.kullaniciAdi, yeniYoneticiAdi);
    strcpy(NYonetici.sifre, yenisifre);
    strcpy(NYonetici.rol, "Yonetici");
    fwrite(&NYonetici, sizeof(Kullanicilar), 1, kullanicilarDosya);
    fclose(kullanicilarDosya);
    printf("Yonetici basariyla eklendi!!\n");
    return 1;
}

int KitapEkleme(int* Kontrol, int* sayac, int* devam) {//Kitap sayacını ve benzersiz ID için Kontrol dizisini parametre olarak alır ve kitap kaydına devam etmek için ise devam parametresini kullanir.
    FILE* kitaplarDosya = fopen("kitaplar1.bin", "ab");
    if (kitaplarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        fclose(kitaplarDosya);
        return 0;
    }
    randNum = rand()%9000+1000;//randNum başlangıcta 0 olduğu için ona bir rastgele sayı atama işlemi.
    Kitaplar ekle;

    printf("Kitap adini giriniz:\n");
    fgets(ekle.kitapAdi, sizeof(ekle.kitapAdi), stdin);
    ekle.kitapAdi[strcspn(ekle.kitapAdi, "\n")] = 0;
    printf("Kitabin yazarinin adini giriniz:\n");
    fgets(ekle.Yazar, sizeof(ekle.Yazar),stdin);
    ekle.Yazar[strcspn(ekle.Yazar, "\n")] = 0;
    printf("Kitap kategorisini giriniz (roman,egitici,tarih,gezi,fantastik):\n");
    fgets(ekle.Kategori, sizeof(ekle.Kategori), stdin);
    ekle.Kategori[strcspn(ekle.Kategori, "\n")] = 0;
    kategoriKontrol(ekle.Kategori);

    while (!isBenzersiz(Kontrol, sayac, randNum)){//Eger az önceki tanımmlanan randNum kontrol dizisinde var ise başka bir ranNum tanımlamak için fonksiyonun içine girer. Fonksiyon, dizinin içinde olmayan benzersiz bir ID bulana kadar devam eder.
        randNum = rand() % 9000+1000;
    }

    ekle.ID = randNum;
    Kontrol[*sayac] = randNum;
    (*sayac)++;

    printf("Kitap durumu (Odunc-Rafta): ");
    scanf("%7s", ekle.kDurum);

    while (strcmp(ekle.kDurum, "Odunc") != 0 && strcmp(ekle.kDurum, "Rafta") != 0) {//Kitabin durumunun yanlış girilmiş olması durumunda tekrar eden bir döngü.
        printf("Gecersiz durum! Tekrar giriniz (Odunc-Rafta): ");
        scanf("%7s", ekle.kDurum);
    }

    fwrite(&ekle, sizeof(Kitaplar), 1, kitaplarDosya);
    fclose(kitaplarDosya);
    printf("Kitap basariyla eklendi. Kitap ID: %.4d\n", ekle.ID);
    printf("Kayda devam etmek istiyor musunuz? (1-Evet, 2-Hayir): ");
    scanf("%d", devam);
    return 1;
}

void kitapArama() {//ID, Kitap Adı, Yazar adı veya kategoriye göre kitap arama fonksiyonu.
    FILE* kitaplarDosya = fopen("kitaplar1.bin", "rb");
    if (kitaplarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        fclose(kitaplarDosya);
        return;
    }

    Kitaplar arama;
    int aramaSecimi;
    char arananKelime[40];
    int arananID;
    int aramadevam = 1;

    while (aramadevam) {//Arka arkaya arama yapabilmek için bir döngü.
        int bulundu = 0;
        clearScreen();
        printf("\nArama Turu Secin:\n1-Kitap ID\n2-Kitap Adi\n3-Yazar Adi\n4-Kategori\n");
        scanf("%d", &aramaSecimi);
        getchar();

        rewind(kitaplarDosya);//Arka arkaya yapılan aramalarda imleci başa getirmek için.

        switch (aramaSecimi) {
            case 1:
                clearScreen();
                printf("Kitap ID giriniz: ");
                scanf("%d", &arananID);
                getchar();
                while (fread(&arama, sizeof(Kitaplar), 1, kitaplarDosya)) {
                    if (arama.ID == arananID) {
                        printf("Kitap bulundu:\nYazar: %s\nAd: %s\nID: %d\nKategori: %s\nDurum: %s\n",
                           arama.Yazar, arama.kitapAdi, arama.ID, arama.Kategori, arama.kDurum);
                        bulundu = 1;
                        break;
                    }
                }
                if (!bulundu) printf("Kitap bulunamadi!\n");
                bulundu=0;
                break;
            case 2:
                clearScreen();
                printf("Kitap adini giriniz: ");
                fgets(arananKelime, sizeof(arananKelime), stdin);
                arananKelime[strcspn(arananKelime, "\n")] = 0;
                while (fread(&arama, sizeof(Kitaplar), 1, kitaplarDosya)) {
                    if (strcmp(arama.kitapAdi, arananKelime) == 0) {
                        printf("Kitap bulundu:\nAd: %s\nID: %d\nKategori: %s\nDurum: %s\n\n",
                            arama.kitapAdi, arama.ID, arama.Kategori, arama.kDurum);
                        bulundu = 1;
                        break;
                    }
                }
                if (!bulundu) printf("Kitap bulunamadi!\n");
                bulundu=0;
                break;
            case 3:
                clearScreen();
                printf("Yazar adi giriniz: ");
                fgets(arananKelime, sizeof(arananKelime),stdin);
                arananKelime[strcspn(arananKelime, "\n")] = 0;
                while(fread(&arama, sizeof(Kitaplar),1,kitaplarDosya)){
                    if(strcmp(arama.Yazar,arananKelime) == 0){
                        printf("\nAd: %s\nID: %d\nYazar Adi: %s\nKategori: %s\nDurum: %s\n\n",
                        arama.kitapAdi, arama.ID, arama.Yazar, arama.Kategori, arama.kDurum);
                        bulundu = 1;
                    }
                }
                if(!bulundu) printf("Bu yazarin herhangi bir kitabi bulunamadi!\n");
                bulundu=0;    
                break;
            case 4:
                clearScreen();
                printf("Kategori giriniz: ");
                fgets(arananKelime, sizeof(arananKelime), stdin);
                arananKelime[strcspn(arananKelime, "\n")] = 0;
                printf("\nSonuclar:\n");
                while (fread(&arama, sizeof(Kitaplar), 1, kitaplarDosya)) {
                    if (strcmp(arama.Kategori, arananKelime) == 0) {
                        printf("Ad: %s\nID: %d\nKategori: %s\nDurum: %s\n\n",
                            arama.kitapAdi, arama.ID, arama.Kategori, arama.kDurum);
                        bulundu = 1;
                    }
                }
                if (!bulundu) printf("Bu kategoride kitap bulunamadi.\n");
                bulundu=0;
                break;
            default:
                printf("Gecersiz secim.\n");
        }

        printf("Aramaya devam etmek istiyor musunuz? (1-Evet, 0-Hayir): ");
        scanf("%d", &aramadevam);
        getchar();
        clearScreen();
    }

    fclose(kitaplarDosya);
}
void kitaplariListele(){    //Bütün kitaplari listeler.
    FILE *kitaplarDosya = fopen("kitaplar1.bin", "rb");
    if (kitaplarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        fclose(kitaplarDosya);
        return;
    }

    Kitaplar kitap;
    int sayac = 0;

    printf("\n--- Tum Kitaplar ---\n");
    while (fread(&kitap, sizeof(Kitaplar), 1, kitaplarDosya)) {
        printf("ID: %d\nAd: %s\nYazar Adi: %s\nKategori: %s\nDurum: %s\n\n",
            kitap.ID,kitap.kitapAdi,kitap.Yazar, kitap.Kategori, kitap.kDurum);
        sayac++;
    }
    printf("%d tane kitap listelendi!\n",sayac);
    if (sayac == 0) {
        printf("Hic kitap bulunamadi.\n");
    }

    fclose(kitaplarDosya);
}
int kitapSil(int *Kontrol) {//ID ile seçilen kitabi gecici dosyaya yazmayarak silme islemi yapar.
    FILE *kitaplarDosya = fopen("kitaplar1.bin", "rb");
    FILE *geciciDosya = fopen("gecici.bin", "wb");
    if (kitaplarDosya == NULL || geciciDosya == NULL) {
        printf("Dosya acilamadi!\n");
        if(kitaplarDosya) fclose(kitaplarDosya);
        if(geciciDosya) fclose(geciciDosya);
        return 0;
    }

    int silinecekID;
    printf("Silmek istediginiz kitap ID'sini girin: ");
    scanf("%d", &silinecekID);

    Kitaplar kitap;
    int bulundu = 0;

    while (fread(&kitap, sizeof(Kitaplar), 1, kitaplarDosya)) {
        if (kitap.ID == silinecekID) {
            bulundu = 1; // Silinecek kitap bulundu
            continue;    // Bu kaydı yazma (sil)
        }
        fwrite(&kitap, sizeof(Kitaplar), 1, geciciDosya);
    }

    fclose(kitaplarDosya);
    fclose(geciciDosya);

    if (bulundu) {
        remove("kitaplar1.bin");
        rename("gecici.bin", "kitaplar1.bin");
        printf("Kitap basariyla silindi.\n");
        return 1;
    } else {
        remove("gecici.bin");
        printf("Kitap bulunamadi.\n");
        return 0;
    }
}
void kitapOduncAlma(char *GirisAdi){//Kullanici adina kitap ID si ile kayıt yaparak kitap durumunu günceller.
    FILE *OduncDosya = fopen("Odunc.bin","ab+");
    FILE *kitaplarDosya = fopen("kitaplar1.bin","rb+");
    Kitaplar ara; 
    oduncKitaplar oduncAra;
    int arananID;
    int bulundu=0;
    int oduncSayisi=0;
    int indexCounter=0,arananindex;//kitaplar1.bin dosyasındaki kitabın konumunu bulmak ve kitap durumunu güncellemek için index değişkenleri.
    time_t timeNw=time(NULL);   //1970 den bu yana olan zamanı saniye cinsinden timeNw değiskenine atar.
    struct tm* zamanPtr = localtime(&timeNw);   //timeNw degiskenindeki zamanı yerel saat ile okunabilir hale getirir.
    printf("Lutfen Odunc almak istediginiz kitabin ID'sini giriniz!\n");
    scanf("%d",&arananID);
    getchar();
    if(OduncDosya==NULL||kitaplarDosya==NULL){
        printf("Dosya acilamadi!!\n");
        if(OduncDosya) fclose(OduncDosya);
        if(kitaplarDosya) fclose(kitaplarDosya);
        return;
    }
    while(fread(&ara, sizeof(Kitaplar),1,kitaplarDosya)){
        
        if(ara.ID==arananID){
            bulundu=1;
            if(strcmp(ara.kDurum,"Odunc")==0){//Aranan kitabin durum kontrolü.
            printf("Kitap Rafta bulunmamaktadir.\n");
            fclose(kitaplarDosya);
            fclose(OduncDosya);
            return;
        }
            else{
            printf("Aradiginiz kitap Raftadir.\n");
            arananindex=indexCounter;
        }
    }
    indexCounter++;
    }
    if(bulundu==0){
        printf("Aradiginiz kitap bulunamadi!\n");
        return;
    }
    while(fread(&oduncAra, sizeof(oduncKitaplar),1,OduncDosya)){
        if(strcmp(GirisAdi,oduncAra.KullaniciAdi)==0){
            oduncSayisi++;
        }
    }
    if(oduncSayisi>=3){
        printf("En fazla 3 tane kitap odunc alabilirsiniz!!\n");
        fclose(kitaplarDosya);
        fclose(OduncDosya);
        return;
    }
    fseek(kitaplarDosya,(arananindex)*sizeof(Kitaplar),SEEK_SET);//Guncellenecek kitabin indexine imleci getirir.
    fread(&ara,sizeof(Kitaplar),1,kitaplarDosya);//Bir structlık okuma yapar.
    strcpy(ara.kDurum,"Odunc");
    fseek(kitaplarDosya, -(long)sizeof(Kitaplar), SEEK_CUR);//Imleci Bir kayıt Sola Kaydırıp yazdırıyoruz.
    fwrite(&ara, sizeof(Kitaplar),1,kitaplarDosya);

    printf("%d ID li kitap en fazla 15 gun sureyle %d:%d:%d tarihinde odunc verilmistir!\n",arananID,zamanPtr->tm_year+1900,zamanPtr->tm_mon+1,zamanPtr->tm_mday);
    oduncAra.alisTarihi=timeNw;//Odunc alma tarihi şimdiki zamana esitlenir.
    oduncAra.KitapID=arananID;
    strcpy(oduncAra.KullaniciAdi,GirisAdi);
    fseek(OduncDosya, 0, SEEK_END);//Kaydı dosyanın sonuna eklemek için imleci dosya sonuna getiriyoruz.
    fwrite(&oduncAra, sizeof(oduncKitaplar),1,OduncDosya);//Odunc.bin dosyasına yazdırıyoruz.
    fclose(kitaplarDosya);
    fclose(OduncDosya);
}
void oduncKitaplariListele(char *GirisAdi){//Kullanicinin odunc aldıgı kitaplari listeler.
    FILE *OduncDosya = fopen("Odunc.bin", "rb");
    oduncKitaplar arama;
    time_t simdi = time(NULL);//Kitap teslimine kalan süreyi hesaplamak için sorgunun yapıldıgı zamanı simdi değişkenine atarız.
    struct tm *zamanPtr;
    int sayac = 0;

    if (OduncDosya == NULL) {
        printf("Dosya Acilamadi!\n");
        fclose(OduncDosya);
        return;
    }

    while (fread(&arama, sizeof(oduncKitaplar), 1, OduncDosya)) {
        if (strcmp(GirisAdi, arama.KullaniciAdi) == 0) {
            zamanPtr = localtime(&arama.alisTarihi);
            int farkGun = (int)((simdi - arama.alisTarihi) / (60 * 60 * 24));//Son teslime kalan gün sayisi.

            printf("%s - %d ID'li kitap, %d:%d:%d tarihinde odunc alinmistir!\n",arama.KullaniciAdi, arama.KitapID,zamanPtr->tm_year + 1900,zamanPtr->tm_mon + 1,zamanPtr->tm_mday);
            printf("Bu kitabi %d gun icinde iade etmek zorundasiniz! (Kalan: %d gun)\n",15, 15 - farkGun);//Degisken yıl olarak 1900 den itibaren gecen yılı tuttugu için +1900 yapıyoruz ve ay içinse index numarasına 1 ekliyoruz.
            sayac++;
        }
    }
    if (sayac == 0) {
        printf("Bu kullaniciya ait odunc kitap bulunamadi.\n");
    }
    fclose(OduncDosya);
}
void teslimEdilmeyenler(){//Teslim süresi dolan bütün kitaplari listeler.
    FILE *OduncDosya = fopen("Odunc.bin","rb");
    oduncKitaplar kontrol;
    time_t simdi=time(NULL);
    int count=0;

    if (OduncDosya == NULL) {
        printf("Dosya acilamadi!\n");
        fclose(OduncDosya);
        return;
    }
    
    while(fread(&kontrol,sizeof(oduncKitaplar),1,OduncDosya)){
        int farkGun = (int)((simdi - kontrol.alisTarihi) / (60 * 60 * 24));
        if(farkGun>=15){
            printf("%s-->%d-->%d\n",kontrol.KullaniciAdi,kontrol.KitapID,farkGun);
            count++;
            
        }
    }
    if(count==0){
        printf("Teslim edilmeyen kitap yoktur!!\n");
    }else{
        printf("%d Kitap listelendi.\n",count);
    }
    fclose(OduncDosya);
}
void uyariKontrol(char *girisAdi){//Kullanicilar giris yaptıktan sonra eger teslim süresi gecen kitapları varsa uyarı alırlar.
    FILE *OduncDosya = fopen("Odunc.bin","rb");

    if (OduncDosya == NULL) {
        printf("Dosya acilamadi!\n");
        fclose(OduncDosya);
        return;
    }
    oduncKitaplar uyari;
    time_t simdi=time(NULL);//Teslim süresini kontrol etmek için simdi zaman değişkeni
    while(fread(&uyari,sizeof(oduncKitaplar),1,OduncDosya)){
        int farkGun = (int)((simdi - uyari.alisTarihi) / (60 * 60 * 24));
        if(strcmp(uyari.KullaniciAdi,girisAdi)==0 && farkGun>=15){
            printf("\n%d ID'li kitabin iade suresi dolmustur. Lutfen en kisa zamanda iade ediniz!!!\n",uyari.KitapID);
        }
    }
    fclose(OduncDosya);
}
void kitapGuncelle(){//ID dısında kitap bilgilerini seçim yaptırarak güncelleyebilir.
    FILE *kitaplarDosya = fopen("kitaplar1.bin", "rb+");
    if (kitaplarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        fclose(kitaplarDosya);
        return;
    }
    Kitaplar guncel;
    int arananID;
    int bulundu = 0;

    printf("Guncellemek istediginiz kitabin ID'sini giriniz: ");
    scanf("%d", &arananID);
    getchar();

    while (fread(&guncel, sizeof(Kitaplar), 1, kitaplarDosya) && !bulundu) {
        if (arananID == guncel.ID) {
            bulundu = 1;
            
            // Kitap bilgilerini bir kez gösterir.
            printf("\nKitap Bilgileri:\n");
            printf("Ad: %s\n", guncel.kitapAdi);
            printf("Yazar: %s\n", guncel.Yazar);
            printf("Durum: %s\n", guncel.kDurum);
            printf("Kategori: %s\n", guncel.Kategori);
            printf("ID: %d\n\n", guncel.ID);

            int secim=0;
             while (secim != 5){
                printf("Ne guncellemek istiyorsunuz?\n");
                printf("1- Isim\n2- Yazar\n3- Durum\n4- Kategori\n5- Cikis\nSecim: ");
                scanf("%d", &secim);
                getchar();

                switch (secim) {
                    case 1:
                        printf("Yeni kitap adi: ");
                        fgets(guncel.kitapAdi, sizeof(guncel.kitapAdi), stdin);
                        guncel.kitapAdi[strcspn(guncel.kitapAdi, "\n")] = 0;
                        break;
                    case 2:
                        printf("Yeni yazar adi: ");
                        fgets(guncel.Yazar, sizeof(guncel.Yazar), stdin);
                        guncel.Yazar[strcspn(guncel.Yazar, "\n")] = 0;
                        break;
                    case 3:
                        printf("Yeni durum (Odunc/Rafta): ");
                        fgets(guncel.kDurum, sizeof(guncel.kDurum), stdin);
                        guncel.kDurum[strcspn(guncel.kDurum, "\n")] = 0;
                        break;
                    case 4:
                        printf("Yeni kategori (roman/gezi/tarih/fantastik/egitici): ");
                        fgets(guncel.Kategori, sizeof(guncel.Kategori), stdin);
                        guncel.Kategori[strcspn(guncel.Kategori, "\n")] = 0;
                        kategoriKontrol(guncel.Kategori);
                        break;
                    case 5:
                        break;
                    default:
                        printf("Gecersiz secim!\n");
                }

                if (secim >= 1 && secim <= 4) {
                    fseek(kitaplarDosya, -(long)sizeof(Kitaplar), SEEK_CUR);//İmlec değiştirilecek kaydın başına alınır.
                    fwrite(&guncel, sizeof(Kitaplar), 1, kitaplarDosya);
                    printf("Guncelleme basarili!\n\n");
                }
            }
           
        }  
    }


    if (!bulundu) {
        printf("Bu ID'ye ait kitap bulunamadi.\n");
    }
    fclose(kitaplarDosya);
}
void kitapIade(char *girisAdi) {//ID girilerek durumu odunc olan kitabi Odunc.bin dosyasından siler ve kitap durumunu günceller.
    FILE *iadeDosya = fopen("Odunc.bin", "rb+");
    FILE *geciciDosya = fopen("GeciciOdunc.bin", "wb+");
    FILE *kitaplarDosya = fopen("kitaplar1.bin", "rb+");
    
    if (iadeDosya == NULL || geciciDosya == NULL || kitaplarDosya == NULL) {
        printf("Dosya acilamadi!\n");
        if (iadeDosya) fclose(iadeDosya);
        if (geciciDosya) fclose(geciciDosya);
        if (kitaplarDosya) fclose(kitaplarDosya);
        return;
    }

    oduncKitaplar iade;
    Kitaplar guncel;
    int iadeID;
    int bulundu = 0;

    printf("Iade etmek istediginiz kitabin ID'sini giriniz: ");
    scanf("%d", &iadeID);
    getchar();

    while (fread(&iade, sizeof(oduncKitaplar), 1, iadeDosya)) {
        if (iadeID == iade.KitapID && strcmp(girisAdi, iade.KullaniciAdi) == 0) {// Kullanıcının kitabı ödünç alıp almadığını kontrol et.
            bulundu = 1;
        } else {
            fwrite(&iade, sizeof(oduncKitaplar), 1, geciciDosya);
        }
    }

    fclose(iadeDosya);
    fclose(geciciDosya);

    if (!bulundu) {//Kitap bulunamadıysa uyarır.
        remove("GeciciOdunc.bin");
        printf("Odunc aldiginiz kitaplardan bu ID de bir kitap bulunamadi!!\n");
        fclose(kitaplarDosya);
        return;
    }

    #ifdef _WIN32
    Sleep(400);//400ms bekle... Windows için
    #else
    usleep(400000); // 400 ms bekle (Linux/macOS)
    #endif


    if(remove("Odunc.bin") != 0){
        printf("Odunc.bin Silinemedi\n");
        fclose(kitaplarDosya);
        return;
    }


    if (rename("GeciciOdunc.bin", "Odunc.bin") != 0) {
        printf("Dosya isleme hatasi!\n");
        printf("Dosya Hata Kodu: %s\n",strerror(errno));
        fclose(kitaplarDosya);
        return;
    }

    
    int kitapGuncellendi = 0;
    while (fread(&guncel, sizeof(Kitaplar), 1, kitaplarDosya)) {//Kitap durumunu güncelleme
        if (iadeID == guncel.ID) {
            strcpy(guncel.kDurum, "Rafta");
            fseek(kitaplarDosya, -(long)sizeof(Kitaplar), SEEK_CUR);
            fwrite(&guncel, sizeof(Kitaplar), 1, kitaplarDosya);
            kitapGuncellendi = 1;
            break;
        }
    }

    if (!kitapGuncellendi) {
        printf("Bir sorun olustu!\n");
    } else {
        printf("Kitap basariyla iade edildi ve durumu guncellendi.\n");
    }

    fclose(kitaplarDosya);
}
void kontrolGuncelle(int *Kontrol,int *kitapSayac){
    (*kitapSayac)=0;
    memset(Kontrol, -1, 500 * sizeof(int));//Kontrol dizisinin içeriğini -1'lerle doldurur
    FILE *kitaplarDosya = fopen("kitaplar1.bin", "rb");//Mevcut Kitap Sayisini Belirleme
    if (kitaplarDosya != NULL) {
    Kitaplar tempBook;
    while (fread(&tempBook, sizeof(Kitaplar), 1, kitaplarDosya)) {
        if (tempBook.ID != 0) {  // ID 0 olanlar silinmiş boş kayıt sayılabilir
            Kontrol[(*kitapSayac)++] = tempBook.ID;  // ID'leri kontrol dizisine ekle ve kitap sayacını kitap sayısına eşitle
        }
    }
    fclose(kitaplarDosya);
    }
}
void kullaniciSayisi(int *userCount){
    (*userCount)=0;
    FILE *kullanicilarDosya = fopen("kullanicilar1.bin","rb");//Mevcut Kullanici Sayisini Belirleme
    if(kullanicilarDosya != NULL){
    Kullanicilar tempUser;
    while(fread(&tempUser, sizeof(Kullanicilar),1,kullanicilarDosya)){
       (*userCount)++;
    }
    } else { 
        printf("Dosya acilamadi!!\n");
        return ;
    }
}
int main() {
    srand(time(NULL));

    int Kontrol[500];
    int secim, devam = 1, kitapSayac = 0, devamKitap = 1, silmeDevam = 1, userCount=0;
    
    memset(Kontrol, -1, sizeof(Kontrol));//Kontrol dizisinin içeriğini -1'lerle doldurur

    char girisAdi[20], girisSifre[20];//Giris yapmak için kullanici adı ve sifre girdisi.
    char ekleAdi[20], ekleSifre[20];//Kullanici veya yönetici eklemek için kullanici adı ve sifre girdisi.

    strcpy(aktifRol,"Temp");//While döngüsüne girebilmek için aktifRol'e "Temp" rol atanması.
    while(strcmp(aktifRol,"Temp")==0){
    
    kontrolGuncelle(Kontrol,&kitapSayac);
    kullaniciSayisi(&userCount);
    
    printf("Kullanici adi: ");
    scanf("%19s", girisAdi);
    printf("Sifre: ");
    scanf("%19s", girisSifre);

    if (!GirisDogrula(girisAdi, girisSifre)) continue;
    clearScreen();
    uyariKontrol(girisAdi);

    if (strcmp(aktifRol, "Admin") == 0 || strcmp(aktifRol, "Yonetici") == 0) {
        devam=1;
        while (devam == 1){        
            printf("\n1-Kullanici Ekle\n2-Kitap Ekle\n3-Kitap Silme\n4-Kitap Guncelle\n5-Kitap Ara\n6-Kitaplari Listele\n7-Kitap Odunc Alma\n8-Odunc Kitaplari Listele\n9-Teslim Edilmeyen Kitaplari Listele\n10-Kitap Iade\n11-Yonetici Ekle\n0-Cikis\nSeciminiz: ");
            scanf("%d", &secim);
            getchar();
            clearScreen();
            switch (secim) {
                case 1:
                    if(userCount>=100){
                        printf("Maksimum Kullanici Sinirina Erisildi!!\n");
                        continue;
                    }
                    printf("Kullanici adi: ");
                    scanf("%19s", ekleAdi);
                    printf("Sifre: ");
                    scanf("%19s", ekleSifre);
                    if(kullaniciEkle(ekleAdi, ekleSifre) == 1){
                        userCount++;
                    }
                    break;
                case 2:
                    while (devamKitap == 1 && kitapSayac < 500) {
                        clearScreen();
                        KitapEkleme(Kontrol, &kitapSayac, &devamKitap);
                        getchar();
                    }
                    devamKitap=1;
                    break;
                case 3:
                    silmeDevam=1;
                    while(silmeDevam==1){
                        clearScreen();
                        kontrolGuncelle(Kontrol,&kitapSayac);
                        if(kitapSil(Kontrol)){
                            kitapSayac--;
                            kontrolGuncelle(Kontrol,&kitapSayac);
                        }
                        printf("Silme islemine devam etmek istiyor musunuz?\n");
                        printf("1-Evet\n0-Hayir");
                        scanf("%d",&silmeDevam);
                        while(silmeDevam!=1&&silmeDevam!=0){
                            printf("Gecerli bir secenek giriniz!\n");
                            scanf("%d",&silmeDevam);
                        }
                    }
                    break;
                case 4:
                    kitapGuncelle();
                    break;
                case 5:
                    kitapArama();
                    break;
                case 6:
                    kitaplariListele();
                    break;
                case 7:
                    kitapOduncAlma(girisAdi);
                    break;
                case 8:
                    oduncKitaplariListele(girisAdi);
                    break;
                case 9:
                    teslimEdilmeyenler();
                    break;
                case 10:
                    kitapIade(girisAdi);
                    break;
                case 11:
                if(strcmp(aktifRol,"Admin") == 0){
                    if(userCount>=100){
                        printf("Maksimum Kullanici Sinirina Erisildi!!\n");
                        continue;
                    }
                    printf("Yonetici adi: ");
                    scanf("%19s", ekleAdi);
                    printf("Sifre: ");
                    scanf("%19s", ekleSifre);
                    if(yoneticiEkle(ekleAdi, ekleSifre) == 1){
                        userCount++;
                    }
                } else {
                    printf("Bunu yapmaya yetkiniz yok!!\n");
                }
                break;
                /*case 12:                           Kontrol dizisinin içeriğini kontrol etmek için kullanıldı.
                    for(int i=0;i<500;i++){
                        printf("%d--",Kontrol[i]);
                    }
                    break;*/
                case 0:
                    strcpy(aktifRol,"Temp");
                    devam=0;
                    break;
                default:
                    printf("Gecersiz secim!\n\n");
                }
        }
    }

    if (strcmp(aktifRol, "Kullanici") == 0) {
        while (1) {
            printf("\n1-Kitap Ara\n2-Kitaplari Listele\n3-Kitap Odunc Alma\n4-Odunc Aldigim Kitaplari Listele\n5-Kitap Iade\n6-Cikis\nSeciminiz: ");
            scanf("%d", &secim);
            printf("\n\n");
            clearScreen();
            if (secim == 1) {
                kitapArama();
                continue;
            } else if (secim == 2) {
                kitaplariListele();
                continue;
            } else if (secim == 3) {
                kitapOduncAlma(girisAdi);
                continue;
            }else if (secim == 4) {
                oduncKitaplariListele(girisAdi);
                continue;
            }else if (secim == 5) {
                kitapIade(girisAdi);
                continue;
            }else if (secim == 6) {
                strcpy(aktifRol,"Temp");
                break;
            } else {
                printf("Gecersiz secim.\n");
                continue;
            }
        }
    }
    }
    return 0;
}