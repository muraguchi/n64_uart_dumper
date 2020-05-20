#! /usr/bin/python
import serial
import struct
import time 
import config
import argparse


parser = argparse.ArgumentParser(description='N64 dumper')
parser.add_argument('mbits',metavar='N',type=int, help='ROM size in Mbits')
args = parser.parse_args()

baseAddr = 0x10000000

def single_read(serial,addr):
    serial.write (("SR%08X" %( addr&0xffffffff )).encode())
    return int(serial.read(4).decode(),16)

ser = serial.Serial(config.SERIAL_DEVICE,
                    config.SERIAL_BAUDRATE,
                    timeout=config.SERIAL_TIMEOUT_SEC)


# get title
header_title = ""
for addr in range(0x20,0x20+20,2):
    rddata = single_read(ser,baseAddr+ addr)
    header_title+=chr(rddata>>8)
    header_title+=chr(rddata&0xff)

header_title=header_title.replace('\x00',"")
header_title=header_title.rstrip()
header_title=header_title.replace('\xa1',"。")
header_title=header_title.replace('\xa2',"「")
header_title=header_title.replace('\xa3',"」")
header_title=header_title.replace('\xa4',"、")
header_title=header_title.replace('\xa5',"・")
header_title=header_title.replace('\xa6',"ヲ")
header_title=header_title.replace('\xa7',"ァ")
header_title=header_title.replace('\xa8',"ィ")
header_title=header_title.replace('\xa9',"ゥ")
header_title=header_title.replace('\xaa',"ェ")
header_title=header_title.replace('\xab',"ォ")
header_title=header_title.replace('\xac',"ャ")
header_title=header_title.replace('\xad',"ュ")
header_title=header_title.replace('\xae',"ョ")
header_title=header_title.replace('\xaf',"ッ")
header_title=header_title.replace('\xb0',"ー")
header_title=header_title.replace('\xb1',"ア")
header_title=header_title.replace('\xb2',"イ")
header_title=header_title.replace('\xb3',"ウ")
header_title=header_title.replace('\xb4',"エ")
header_title=header_title.replace('\xb5',"オ")
header_title=header_title.replace('\xb6',"カ")
header_title=header_title.replace('\xb7',"キ")
header_title=header_title.replace('\xb8',"ク")
header_title=header_title.replace('\xb9',"ケ")
header_title=header_title.replace('\xba',"コ")
header_title=header_title.replace('\xbb',"サ")
header_title=header_title.replace('\xbc',"シ")
header_title=header_title.replace('\xbd',"ス")
header_title=header_title.replace('\xbe',"セ")
header_title=header_title.replace('\xbf',"ソ")
header_title=header_title.replace('\xc0',"タ")
header_title=header_title.replace('\xc1',"チ")
header_title=header_title.replace('\xc2',"ツ")
header_title=header_title.replace('\xc3',"テ")
header_title=header_title.replace('\xc4',"ト")
header_title=header_title.replace('\xc5',"ナ")
header_title=header_title.replace('\xc6',"ニ")
header_title=header_title.replace('\xc7',"ヌ")
header_title=header_title.replace('\xc8',"ネ")
header_title=header_title.replace('\xc9',"ノ")
header_title=header_title.replace('\xca',"ハ")
header_title=header_title.replace('\xcb',"ヒ")
header_title=header_title.replace('\xcc',"フ")
header_title=header_title.replace('\xcd',"ヘ")
header_title=header_title.replace('\xce',"ホ")
header_title=header_title.replace('\xcf',"マ")
header_title=header_title.replace('\xd0',"ミ")
header_title=header_title.replace('\xd1',"ム")
header_title=header_title.replace('\xd2',"メ")
header_title=header_title.replace('\xd3',"モ")
header_title=header_title.replace('\xd4',"ヤ")
header_title=header_title.replace('\xd5',"ユ")
header_title=header_title.replace('\xd6',"ヨ")
header_title=header_title.replace('\xd7',"ラ")
header_title=header_title.replace('\xd8',"リ")
header_title=header_title.replace('\xd9',"ル")
header_title=header_title.replace('\xda',"レ")
header_title=header_title.replace('\xdb',"ロ")
header_title=header_title.replace('\xdc',"ワ")
header_title=header_title.replace('\xdd',"ン")
header_title=header_title.replace('\xde',"゛")
header_title=header_title.replace('\xdf',"゜")
header_title=header_title.replace("ウ゛","ヴ")
header_title=header_title.replace("カ゛","ガ")
header_title=header_title.replace("キ゛","ギ")
header_title=header_title.replace("ク゛","グ")
header_title=header_title.replace("ケ゛","ゲ")
header_title=header_title.replace("コ゛","ゴ")
header_title=header_title.replace("サ゛","ザ")
header_title=header_title.replace("シ゛","ジ")
header_title=header_title.replace("ス゛","ズ")
header_title=header_title.replace("セ゛","ゼ")
header_title=header_title.replace("ソ゛","ゾ")
header_title=header_title.replace("タ゛","ダ")
header_title=header_title.replace("チ゛","ヂ")
header_title=header_title.replace("ツ゛","ヅ")
header_title=header_title.replace("テ゛","デ")
header_title=header_title.replace("ト゛","ド")
header_title=header_title.replace("ハ゛","バ")
header_title=header_title.replace("ヒ゛","ビ")
header_title=header_title.replace("フ゛","ブ")
header_title=header_title.replace("ヘ゛","ベ")
header_title=header_title.replace("ホ゛","ボ")
header_title=header_title.replace("ハ゜","パ")
header_title=header_title.replace("ヒ゜","ピ")
header_title=header_title.replace("フ゜","プ")
header_title=header_title.replace("ヘ゜","ペ")
header_title=header_title.replace("ホ゜","ポ")

print (header_title)

outfile=header_title.replace('-','_').replace('/','_').replace('\"','_').replace('\?','_').replace(' ','_').replace("\'","_")+".z64"
n64f=open((outfile),"wb")

for i_mb in range(0,args.mbits):
    ser.write(( "PR%04X" %( (baseAddr>>16)+(i_mb<<1) )).encode())
    print( "PR%04X" %( (baseAddr>>16)+(i_mb<<1) ))
    for i_data in range(128*1024):
        #data = int(ser.read(2).decode(),16)
        data = ser.read(1)
        n64f.write(data)
    
quit()
