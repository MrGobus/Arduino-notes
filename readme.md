# Arduino pro micro - записки исследователя #

[Arduino pro micro](http://www.pighixxx.com/test/2016/07/pro-micro-pinout/) - аналог arduino на чипе ATmega32U4, идентифицируется как "Arduino Leonardo". При заказе мне чертовски повезло, хотел брать "arduino pro mini" но ее не оказалось и выбор пал на эту модель. Как выяснилось позже - это единственный чип который официально может делать вывод через USB как HID устройство (эмуляция клавиатуры и мыши, для других плат потребуется танцы с бубнами).

Критерии выбора были:
1. Цена. В лом было платить 1000 рублей за неведомо что. Итого: 325 рублей а Амперкоте.
2. Наличие USB порта. Выяснилось, что есть модели без USB, для прошивки которых потребуется программатор.
3. Наличие впаянных ножек. К сожалению для arduino pro micro ножки прилагались отдельно, они небыли припаяны, пришлось паять самому...

Также были заказаны:
1. Монтажная палата.
2. Набор соединительных проводов разных типов (как показала практика проводки папа-папа востребованы более всего, их стоит брать побольше).
3. Пульт ДУ.
4. Кнопочки разных цветов - 3 штуки.
5. Модуль для чтения RFID меток - такие чипы в виде спиральки которые вклеивают в книги, одежу и т.п. в магазинах чтобы не сперли чего, также RFID использует мосгортранс в билетиках, но главная тема ради чего все это мутится, аутентификация компьютера (ввод пароля) от брелока (прилагается к модулю) или от карты. Именно тут понадобится особенность чипа ATmega32U4 работать как HID устройство, то есть приложили брелок оно напечатало пароль.

Позже оказалось, что неплохо было бы закупить:
1. Резисторы на 10кОм для подключения кнопок (можно и без них, но рекомендуют).
2. Резисторы на 220Ом для подключения светодиодов.
3. Светодиоды чтобы моргать, хотя имеются встроенные, так что можно и без них.

# Подключение #

Обычным микро usb кабелем подключаем к компьютеру. Windows брякает и определяет ее как "Arduino Leonardo" (в диспетчере устройств раздел "Порты COM и LPT")

Ставим [Arduino IDE](https://www.arduino.cc/en/main/software)

В меню "Инструменты" выбираем "Arduino Leonardo" и порт нашей платы. Порт может изменится. В случае изменения порта, порт надо выбрать заново вручную.

После этого плата готова к загрузке программ.

# blink - моргаем встроенными светодиодами #

В классической Arduino встроенный светодиод расположен на пине 13 и им можно моргать, для чего есть классический пример под названием blink входящий в состав Arduino IDE. В ATmega32U4 такого светодиода нет, и стандартный пример blink работать не будет. Зато имеются два светодиода служащими индикаторами для процессов чтения записи RX и TX. Они расположены на пинах 17 (RX) и 30 (TX), так что напишем свой пример моргающий этими светодиодами.

Также добавим функционал моргания внешним светодиодом подключенным по классической схеме на пин 2.

![GitHub Logo](/images/scheme1.png)

По какой то, видимо технологической, причине включение и выключение встроенных светодиодов инвертировано и значение LOW включает светодиод а значение HIGH выключает. В случае с внешним светодиодом все наоборот.

Код примера: [blink](./blink)
