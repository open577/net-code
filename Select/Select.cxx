#include <iostream>
#include <unistd.h>
#include <fcntl.h>

// int main()
// {
//     std::cout<<sizeof(fd_set)*8<<std::endl;
//     return 0;
// }

// //非阻塞
// int main()
// {
//     while(true)
//     {
//         char buff[1024];
//         ssize_t n=read(0,buff,sizeof(buff));
//         if(n>0)
//         {
//             buff[n]=0;
//             std::cout<<buff<<std::endl;
//         }

//         else if(n==0)
//         {
//             std::cout<<"退出"<<std::endl;
//             break;
//         }

//         else
//         {
//             std::cout<<"读取出错"<<std::endl;
//             break;
//         }
//     }
//     return 0;
// }

// 阻塞

void SetNonBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        std::cout<<"Select fcntl"<<std::endl;

        perror("fcntl file\n");
        return;
    }

    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

int main()
{
    SetNonBlock(0);
    char buff[1024];

    while (true)
    {
        sleep(1);

        ssize_t n = read(0, buff, sizeof(buff));
        if (n > 0)
        {
            buff[n-1] = 0;
            std::cout << buff << std::endl;
            std::cout << "\r\n";
            continue;
        }

        else if (n == 0)
        {
            std::cout << "连接断开" << std::endl;
            break;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                std::cout << "数据还没准备好" << std::endl;
                continue;
            }

            else if (errno == EINTR)
            {
                std::cout << "被信号打断" << std::endl;
                continue;
            }

            else
            {
                std::cout << "读取数据失败" << std::endl;
                break;
            }
        }

    }
    return 0;
}