
#include "lib_io.h"


static char buffer[512];

int32  OS_printf( uint32 p_out_target, const char *p_format, ... )
{
    OS_Assert( IS_LIB_DEF_IO_OUT(p_out_target) );

    uint32      t_i;
    uint32      t_counter = 0;
    va_list     t_args;

    const char  *t_format;

    va_start( t_args, p_format );
    for( t_format = p_format ; *t_format ; t_format++ )
    {
        if( *t_format != '%' )
        {
            buffer[t_counter] = *t_format;
            t_counter++;
        }
        else
        {
            t_format++;
            switch( *t_format )
            {
                case 'd':
                    t_counter += sprintf( &buffer[t_counter], "%d", va_arg(t_args,int) );
                    break;

                case 'c':
                    t_counter += sprintf( &buffer[t_counter], "%c", va_arg(t_args,int) );
                    break;

                case 's':
                    t_counter += sprintf( &buffer[t_counter], "%s", (char *)va_arg(t_args,int) );
                    break;

                case '%':
                    buffer[t_counter] = '%';
                    t_counter++;
                    break;
            }
        }
    }
    va_end(t_args);

    switch( p_out_target )
    {
        case LIB_DEF_IO_OUT_USART1:
            for( t_i = 0; t_i < t_counter; t_i++ )
            {
                BSP_usart1_fputc( buffer[t_i] );
            }
            break;

        case LIB_DEF_IO_OUT_USART2:
            for( t_i = 0; t_i < t_counter; t_i++ )
            {
                BSP_usart2_fputc( buffer[t_i] );
            }
            break;

        case LIB_DEF_IO_OUT_USART1 | LIB_DEF_IO_OUT_USART2:
            for( t_i = 0; t_i < t_counter; t_i++ )
            {
                BSP_usart1_fputc( buffer[t_i] );
                BSP_usart2_fputc( buffer[t_i] );
            }
            break;
    }
    return t_counter;
}

