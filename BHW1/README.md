Королёв Владислав Юрьевич, БПИ216, Вариант 27
Решение на 4 балла:
Программа реализует конвейерную обработку текстового файла. Сначала файл считывается, и его содержимое записывается в один конец канала (pipe). Затем другой процесс считывает данные из этого конца канала, выполняет обработку (подсчитывает частоту вхождений keyword в строку) и записывает результат во второй конец канала. Третий процесс читает данные из этого конца канала и записывает их в выходной файл.
Решение задачи включает следующие шаги:
1. Открытие и чтение входного файла в первом процессе.
2. Создание первого канала для связи между первым и вторым процессами.
3. Создание второго канала для связи между вторым и третьим процессами.
4. Создание 2-ух дочерних процессов, которые будут обрабатывать данные из первого и второго каналов.
5. Запись данных в первый канал из первого процесса.
6. Чтение данных из первого канала вторым процессом, выполнение обработки и запись результатов во второй канал.
7. Чтение данных из второго канала третьим процессом и запись их в выходной файл.
8. Ожидание завершения работы дочерних процессов, закрытие всех каналов и файловых дескрипторов.


Результаты работы программы представлены в папке tests.
