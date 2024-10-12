from BTinterface import BTInterface
from pixelated import pixalated
from Send_Period import PeriodString
import time

BT_name = "/dev/tty.CAR-10"
img_path = '/Users/lijuan/Desktop/ru,6wj6/截圖 2024-05-28 上午10.57.40.png'
Mode = 0


def main():
    print("test")
    if Mode == 0: #main code
        interface = BTInterface(BT_name)
        pixel = pixalated(img_path)
        period_string = PeriodString(pixel)
        time.sleep(5)
        interface.send_period("start")
        print("start")
        time.sleep(1)
        Period_sent : str
        ask_command : str
        row_index = -1
        while row_index < 99 :
            ask_command = interface.bt.serial_read_byte()
            if ask_command == "636f6d6d616e64":
                print("command")
                row_index += 1
                Period_sent = period_string.convert_to_string(row_index , pixel)
                interface.send_period(Period_sent)

    elif Mode == 1: #BT_testing
        interface = BTInterface(BT_name)
        time.sleep(5)
        T = "start"
        interface.send_period(T)
        while True:
            command = interface.bt.serial_read_byte()
            if command == "636f6d6d616e64":
                print("command")

    elif Mode == 2: #turn img to Period array
        interface = BTInterface(BT_name)
        pixel = pixalated(img_path)
        period_string = PeriodString(pixel)
        for i in range (0 , 100):
            print(period_string.convert_to_string(i , pixel))

if __name__ == "__main__":
    main()
