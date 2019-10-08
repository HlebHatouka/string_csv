# string_csv

Праграма счытвае з csv-файлу дадзеныя ў фармаце id;partner;number;date;status , ("date" ў фармаце unix timestamp), захоўваючы аб'екты ў boost::multi_index.
Затым стартуе ў парадку id, выводзячы ўсё ў кансоль, пасьля адбірае радок з найвялікшым id, радок, у якога id роўны 100, а таксама радкі, датаваныя 2019-ым годам.

The program reads data from a csv-file in the format id;partner;number;date;status , ("date" is in the format of unix timestamp), saving the objects in the boost::multi_index. Then sorting by id starts, printing everything in the console, then the program selects the line with the greatest id, line with id = 100, and the lines dated 2019 year.

![Прыклад выпаўненьня праграмы](string_csv.png)
