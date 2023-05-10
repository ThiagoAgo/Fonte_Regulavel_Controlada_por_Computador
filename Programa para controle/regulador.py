#####################################################################################
# Prog:
# Finalidade comunicasao serial com um segundo software sendo executado no arduino
# Autor: Thiago Elias dos Santos
# Data 27/04/2023
# Referencias:
# 1 - https://www.youtube.com/playlist?list=PLtcOfE9KWB66LgOjZyJstFAMV7Yywh3K7
######################################################################################

#! /usr/bin/env python
#  -*- coding: utf-8 -*-
#
# GUI module generated by PAGE version 5.1
#  in conjunction with Tcl version 8.6
#    Apr 27, 2023 02:54:20 PM -03  platform: Windows NT

import sys
from tkinter import *
from time import sleep
import threading
import serial
import serial.tools.list_ports
comunicacao = serial.Serial()
top = Tk()

Porta = "COMx"
TaxaBits = "9600"
Msg = "Escolha a porta e a velocidade de comunicação"
CorMsg = "#009000"
ProcurandoPortas = True

try:
    import Tkinter as tk
except ImportError:
    import tkinter as tk

try:
    import ttk
    py3 = False
except ImportError:
    import tkinter.ttk as ttk
    py3 = True

#import regulador_support

######################################################################################
#_____________________________________Funções________________________________________#

def f_procurando_portas():
    global ProcurandoPortas
    while True:
        sleep(3)
        if ProcurandoPortas:
            textoMsg['text'] ="Procurando por portas ..."
            textoMsg['foreground'] = "#009000"
            portas = serial.tools.list_ports.comports(include_links=False)
            ComboboxPorta['values'] = ""
            i =0
            for porta in sorted(portas):
                ComboboxPorta['values'] = (*ComboboxPorta['values'], porta.device) #porta.device
                i= i+1
                ProcurandoPortas = False
                textoMsg['text'] ="Lista atualizada selecione uma porta."
                textoMsg['foreground'] = "#009000"
                #print(porta.device)


def okBt1():
    global Porta
    Porta = ComboboxPorta.get()
    print(Porta)
    #Porta = ComboboxVelocidade.get(ANCHOR)
    #textoPortaAtual["text"] = Porta
    #textoMenssagem["text"]="Porta: "+Porta+" Velocidade: "+TaxaBits
    
def okBt2():
    global TaxaBits
    TaxaBits = ComboboxVelocidade.get()
    print(TaxaBits)
    #textoVelocidadeAtual["text"] = TaxaBits
    #textoMenssagem["text"]="Porta: "+Porta+" Velocidade: "+TaxaBits


def fbtAtualizar():
    global ProcurandoPortas
    ProcurandoPortas = True

def tensaoPersonalizada(tensao):
    if not comunicacao.is_open:
        textoMsg['text'] ="Primeiro conecte a um dispositivo"
        textoMsg['foreground'] = "#900000"

    else:
        v = round(tensao,3)
        msg = "A0000F"
        vv =int(v * 1000) 
        print(v)
        #textoMenssagem["text"]="Tensão atualizada para:"+str(v)+" V"
        textoMsg['text'] ="Tensão atualizada para: "+str(v)+" V"
        textoMsg['foreground'] = "#000090"
        if v == 0 :
            textoMsg['text'] ="Saida desligada"
            textoMsg['foreground'] = "#000090"
        elif v>=0.1 and v<1:
            msg ="A00"+str(vv)+"F"
        elif v >= 1 and v <10:
            msg ="A0"+str(vv)+"F"
        elif v>=10 and v<65:
            msg ="A"+str(vv)+"F"
        else:
            textoMsg['text'] ="Erro tensão não aceita"
            textoMsg['foreground'] = "#900000"

        comunicacao.write(bytes(msg, 'ascii'))
        #print(msg)
        #print(bytes(msg, 'ascii'))


def iniciarComunicacao():
    okBt1()
    okBt2()
    global comunicacao, Porta, TaxaBits

    if not comunicacao.is_open:
        try:
            comunicacao = serial.Serial(port=Porta, baudrate=TaxaBits)
            textoMsg['text'] ="Comunicação Iniciada!"
            textoMsg['foreground'] = "#000090"
        except IOError:
            comunicacao.close()
            textoMsg['text'] ="Erro ao iniciar Comunicação!"
            textoMsg['foreground'] = "#900000"
    else:
        textoMsg['text'] ="A comunicação já foi iniciada."
        textoMsg['foreground'] = "#009000"

def desligarComunicacao():
    global comunicacao
    comunicacao.close()
    textoMsg['text'] ="Comunicação Encerrada!"
    textoMsg['foreground'] = "#000090"

def esta_aberta():
    global comunicacao
    return comunicacao.is_open()

def escrever_byte(byte):
    if comunicacao.is_open:
        comunicacao.write((byte,))

def ler_linha():
    return comunicacao.readline()

def resetar_buffer_de_entrada():
    if comunicacao.is_open:
        comunicacao.reset_input_buffer()



#######################################################################################
##########################################################################################

# Tread para lista de portas
procurando_portas = threading.Thread(target=f_procurando_portas)
procurando_portas.daemon = True
procurando_portas.start()
###########################################################################################





top.geometry("432x247+373+126")
top.title("Controle de Tensão")
top.minsize(400, 200)
top.maxsize(2962, 1272)
top.resizable(1, 1)
top.title("New Toplevel")
top.configure(background="#d9d9d9")
top.configure(highlightbackground="#d9d9d9")
top.configure(highlightcolor="black")

textoPorta = tk.Label(top)
textoPorta.place(relx=0.046, rely=0.04, height=21, width=74)
textoPorta.configure(activebackground="#f9f9f9")
textoPorta.configure(activeforeground="black")
textoPorta.configure(background="#d9d9d9")
textoPorta.configure(disabledforeground="#a3a3a3")
textoPorta.configure(foreground="#000000")
textoPorta.configure(highlightbackground="#d9d9d9")
textoPorta.configure(highlightcolor="black")
textoPorta.configure(text='''Porta''')

ComboboxPorta = ttk.Combobox(top)
ComboboxPorta.place(relx=0.023, rely=0.121, relheight=0.085
                , relwidth=0.238)
ComboboxPorta.configure(textvariable=Porta)########################################
ComboboxPorta.insert(1," ")
ComboboxPorta.configure(takefocus="")
ComboboxPorta.configure(justify=CENTER)

"""
btOk1 = ttk.Button(top)
btOk1.place(relx=0.046, rely=0.243, height=25, width=76)
btOk1.configure(takefocus="")
btOk1.configure(text='''Ok''')
btOk1.configure(command=okBt1)

btOk2 = ttk.Button(top)
btOk2.place(relx=0.046, rely=0.607, height=25, width=76)
btOk2.configure(takefocus="")
btOk2.configure(text='''Ok''')
btOk2.configure(command=okBt2)
"""

Label1 = tk.Label(top)
Label1.place(relx=0.046, rely=0.25, height=21, width=74)
Label1.configure(activebackground="#f9f9f9")
Label1.configure(activeforeground="black")
Label1.configure(background="#d9d9d9")
Label1.configure(disabledforeground="#a3a3a3")
Label1.configure(foreground="#000000")
Label1.configure(highlightbackground="#d9d9d9")
Label1.configure(highlightcolor="black")
Label1.configure(text='''Velocidade''')


ComboboxVelocidade = ttk.Combobox(top)
ComboboxVelocidade.place(relx=0.023, rely=0.33, relheight=0.085
                , relwidth=0.238)
ComboboxVelocidade.configure(textvariable=TaxaBits)#################################
ComboboxVelocidade['values']=("300","600","1200","2400","9600","19200","38400","57200","128000","256000")
ComboboxVelocidade.configure(takefocus="")
ComboboxVelocidade.configure(justify=CENTER)

bt3v3 = ttk.Button(top)
bt3v3.place(relx=0.37, rely=0.081, height=45, width=76)
bt3v3.configure(takefocus="")
bt3v3.configure(text='''3.3V''')
bt3v3.configure(command=lambda: tensaoPersonalizada(3.3))    # o comando lambda faz com que o comando seja executado 
                                                            # somete quando aperto o botao

bt5v = ttk.Button(top)
bt5v.place(relx=0.579, rely=0.081, height=45, width=76)
bt5v.configure(takefocus="")
bt5v.configure(text='''5V''')
bt5v.configure(command=lambda: tensaoPersonalizada(5))

bt9v = ttk.Button(top)
bt9v.place(relx=0.787, rely=0.081, height=45, width=76)
bt9v.configure(takefocus="")
bt9v.configure(text='''9V''')
bt9v.configure(command=lambda: tensaoPersonalizada(9))

bt12v = ttk.Button(top)
bt12v.place(relx=0.37, rely=0.283, height=45, width=76)
bt12v.configure(takefocus="")
bt12v.configure(text='''12V''')
bt12v.configure(command=lambda: tensaoPersonalizada(12))

bt19v = ttk.Button(top)
bt19v.place(relx=0.579, rely=0.283, height=45, width=76)
bt19v.configure(takefocus="")
bt19v.configure(text='''19V''')
bt19v.configure(command=lambda: tensaoPersonalizada(19))

bt24v = ttk.Button(top)
bt24v.place(relx=0.787, rely=0.283, height=45, width=76)
bt24v.configure(takefocus="")
bt24v.configure(text='''24V''')
bt24v.configure(command=lambda: tensaoPersonalizada(24))

btAtualizar = ttk.Button(top)
btAtualizar.place(relx=0.023, rely=0.45, height=25, width=102)
btAtualizar.configure(takefocus="")
btAtualizar.configure(text='''Atualizar Portas''')
btAtualizar.configure(command=fbtAtualizar)

btDesconectar = ttk.Button(top)
btDesconectar.place(relx=0.37, rely=0.81, height=25, width=76)
btDesconectar.configure(takefocus="")
btDesconectar.configure(text='''Desconectar''')
btDesconectar.configure(command=desligarComunicacao)

btConectar = ttk.Button(top)
btConectar.place(relx=0.787, rely=0.81, height=25, width=76)
btConectar.configure(takefocus="")
btConectar.configure(text='''Conectar''')
btConectar.configure(command=iniciarComunicacao)

textoMsg = ttk.Label(top)
textoMsg.place(relx=0.37, rely=0.7, height=19, width=260)
textoMsg.configure(background="#d9d9d9")
textoMsg.configure(foreground= CorMsg)
textoMsg.configure(font="TkDefaultFont")
textoMsg.configure(relief="flat")
textoMsg.configure(anchor='w')
textoMsg.configure(justify='center')
#textoMsg.configure(textvariable=Msg)
textoMsg.configure(text=Msg)

Scale1 = tk.Scale(top, from_=0.0, to=30.0)
Scale1.place(relx=0.37, rely=0.486, relwidth=0.593, relheight=0.0
                , height=42, bordermode='ignore')
Scale1.configure(activebackground="#ececec")
Scale1.configure(background="#d9d9d9")
Scale1.configure(foreground="#000000")
Scale1.configure(highlightbackground="#d9d9d9")
Scale1.configure(highlightcolor="black")
Scale1.configure(orient="horizontal")
Scale1.configure(resolution="0.1")
Scale1.configure(troughcolor="#d9d9d9")
#Scale1.configure(variable=regulador_support.ten)

btAlterar = ttk.Button(top)
btAlterar.place(relx=0.579, rely=0.81, height=25, width=76)
btAlterar.configure(takefocus="")
btAlterar.configure(text='''Alterar''')
btAlterar.configure(command= lambda: tensaoPersonalizada(Scale1.get()))
print(Scale1.get())
#btAlterar.configure(command=tensaoPersonalizada)



top.mainloop()
