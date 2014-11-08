#include <cstdio>
#include <cstdint>
#include <map>

using namespace std;

uint32_t TransformIntoUint32(FILE *file)
//Превращение нормального ip адреса в 32-битное число
{
    size_t oct0, oct1, oct2, oct3;
    if (fscanf(file,"%u.%u.%u.%u", &oct0, &oct1, &oct2, &oct3)!=4
            || oct1>255 || oct2>255 || oct3>255 || oct0>255)
    {
        printf("Wrong IP address!\n");
        return 0;
    }
    return oct3+(oct2<<8)+(oct1<<16)+(oct0<<24);
}

void TransformIntoIP(FILE *file, uint32_t num)
//Превращение ip адреса из 32-битного числа в 4 октета
{
    //Определяем и заполняем каждый из октетов
    size_t oct0=(num)>>24;
    size_t oct1=((num)>>16)&0xff;
    size_t oct2=((num)>>8)&0xff;
    size_t oct3=(num)&0xff;
    printf("%u.%u.%u.%u", oct0, oct1, oct2, oct3);
    fprintf(file, "%u.%u.%u.%u", oct0, oct1, oct2, oct3);
}

int main(int argc, char* argv[])
{   
    if (argc!=3)        //Проверка на аргументы
    {
        printf("There is a wrong number of arguments!");
        return 1;
    }

    FILE* ip_list=fopen(argv[1], "r");
    if (!ip_list)
    {
        printf("Can't open file");
        return 1;
    }

    FILE* results=fopen(argv[2], "w");
    if (!results)
    {
        printf("Can't open file");
        fclose(ip_list);
        return 1;
    }

    uint32_t ip_begin, user_ip;
    struct value
    {
        uint32_t ip_end;            //конечный ip
        bool used = false;          //флаг, попал ли в этот диапазон введенный ip
    };
    value end;
    map <uint32_t, value> ip;       //Контейнер с начальным и конечным ip
    while ((ip_begin=TransformIntoUint32(ip_list))!=0)
    //Считывание ip адресов из файла
    {
        fscanf(ip_list, " - ");
        end.ip_end= TransformIntoUint32(ip_list);
        ip.emplace(ip_begin, end);
    }
    fclose(ip_list);

    printf("For EOF press:\n\n 1)\"Ctrl+Z\" for Windows.\n 2)\"Ctrl+D\" for Unix.\n\n");
    //определяем итератор поиска в контейнере
    std::map <uint32_t, value> :: iterator search;
    while ((user_ip=TransformIntoUint32(stdin))!=0)
    {
        printf("%u\n", user_ip);
        //поиск числа, большего или равного user_ip
        search=ip.lower_bound(user_ip);

        if (search!=ip.end())
        //Когда итератор не равен послеконечному
        {
            if (search->first==user_ip)
            //Значения по итераторам равны - начало интервала
            {
                search->second.used=true;
                printf("This IP address is included in ranges. (It's begin range).\n");
                continue;
            }
            else
            {
                printf("This IP address isn't included ranges.\n");
                continue;
            }
        }
        if (search!=ip.begin())
        //Когда итератор не равен начальному
        {
            --search;
            if(search->second.ip_end>=user_ip)
            //Случай, когда айпишник попадает в предыдущий интервал
            {
                (search)->second.used=true;
                printf("This IP address is included in ranges. (It is in range).\n");
            }
            else
            //Случай, когда айпишник попадает между интервалами
            {
                printf("This IP address isn't included ranges. (It is between ranges)\n");
            }
        }
    }

    //Вывод диапазонов айпишников, в которые не попали
    //пользовательские запросы
    printf("Unused IP intervals:\n");
    fprintf(results, "Unused IP intervals:\n");
    for(auto &it: ip)
    {
        if (it.second.used==false)
        {
            TransformIntoIP(results, it.first);
            printf(" - ");
            fprintf(results, " - ");
            TransformIntoIP(results, it.second.ip_end);
            printf("\n");
            fprintf(results, "\n");
        }
    }
    fclose(results);
    return 0;
}
