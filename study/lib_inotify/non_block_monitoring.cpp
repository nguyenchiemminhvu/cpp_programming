#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int main()
{
    int fd = inotify_init1( IN_NONBLOCK );
    if ( fd < 0 )
    {
        perror( "inotify_init" );
    }

    int wd = inotify_add_watch( fd, "/tmp", IN_CREATE | IN_DELETE );
    if ( wd == -1 )
    {
        fprintf( stderr, "Cannot watch '/tmp': %s\n", strerror( errno ) );
        exit( EXIT_FAILURE );
    }

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    char buffer[ EVENT_BUF_LEN ];
    while (true)
    {
        int poll_ret = poll( &pfd, 1, -1 );
        if ( poll_ret == -1 )
        {
            perror( "poll" );
            break;
        }

        if (poll_ret == 0)
        {
            continue; // Timeout occurred, should not happen with -1 timeout
        }

        if (pfd.revents & POLLIN)
        {
            int length = read( fd, buffer, EVENT_BUF_LEN );
            if ( length < 0 )
            {
                perror( "read" );
                break;
            }

            int i = 0;
            while ( i < length )
            {
                struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
                if ( event->len )
                {
                    if ( event->mask & IN_CREATE )
                    {
                        printf( "The file %s was created.\n", event->name );
                    }
                    else if ( event->mask & IN_DELETE )
                    {
                        printf( "The file %s was deleted.\n", event->name );
                    }
                }
                i += EVENT_SIZE + event->len;
            }
        }
    }
    
    inotify_rm_watch( fd, wd );
    close( fd );
}