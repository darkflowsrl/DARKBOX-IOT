import time, os, sys

host = "mqtt.darkflow.com.ar"
topics = ["giuli/testing", "giuli/data"]

def publish(topic, message):
    try:
        os.system("mosquitto_pub -h {} -m {} -t {}}".format(host, message, topic))
    except Exception as E:
        print(E)

def subscribe_():
    print("........xxxx........")
    try:
        while True:
            os.system("mosquitto_sub -h {} -t {} ".format(host, "giuli/#"))

    except Exception as E:
        print(E)

def menu():
    os.system("cls")
    print("------------------------\n")
    print("1 > enviar mensaje")
    print("2 > conectarse a: " + host)
    option = input("\n > ")
    if option == "1":
        os.system("cls")
        msg = input("Escribe tu mensaje: ")
        print("Que topico quieres enviar: ")
        for i in topics: print(i + "\n")
        option = input("Publicar al tópico 1 o al 2: ")
        if(option == "1"):
            publish(topics[0], msg)
        elif(option == "2"):
            publish(topics[1], msg)
        else:
            print("Opción incorrecta...")
            time.sleep(3)
    elif option == "2":
        print("........xxxx........")
        subscribe_()
    elif option == "3":
        sys.exit()
    else:
        print("Opción incorrecta....")
        time.sleep(3)

while True:
    try:
        os.system("cls")
        menu()
    except Exception as error:
        print(error)
        time.sleep(3)
        menu()