# 6502エミュレーター

　MOS6502の簡易的なエミュレータです。  
コマンドラインで、MOS6502のマシン語で書かれたプログラムを実行することができます。
 
### 実行プログラム要件

サイズ上限          ：65536byte
リセットベクターアドレス  ：0xfffc - 0xfffd
ファイル名           ：ROM.bin
エミュレーターと同じディレクトリに実行プログラムを配置

### 備考
下記の命令は実装していません。  
・ Decimal Mode  
・ 割り込み関連  
