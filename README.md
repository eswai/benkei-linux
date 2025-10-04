# Benkei for Linux

Benkei for Linuxは大岡俊彦氏が考案されたかな入力方式「薙刀式」のLinux実装です。

http://oookaworks.seesaa.net/article/456099128.html

MozcやAnthy、libkccなど、主要な日本語入力システムに対応します。 
インプットメソッドフレームワークはfcitx5を想定していますが、ibusやuimでも利用可能です。
開発はOmarchy (Arch Linux) とfcitx5で動作確認しています。

# インストール方法
  
ターミナルから以下のコマンドを入力します。

    $ make clean; make; sudo make install

インストールには管理者(root)権限が必要です。  
またGCC、makeが必要ですので、もしエラーになる場合は apt-get などで事前に導入しておいてください。

    $ sudo apt-get install -y make gcc


# 起動方法

    $ sudo -E oyainput [username]

usernameは利用するユーザー名を指定します。  
省略した場合は、現在のログインユーザー名になります。  
もしsuコマンドなどで管理者(root)モードでターミナルにログインしている場合はrootユーザのホームディレクトリを見に行くことになりますので、[username]は省略しないことをおすすめします。

# 謝辞

Benkei for LinuxはINWORKSの勝部新氏が開発されたoyainputをベースに、入力方式を親指シフトから薙刀式に変更しました。
oyainputのソースコードを公開いただいた勝部氏に深く感謝いたします。

https://github.com/inwskatsube/oyainput
