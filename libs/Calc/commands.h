_COMMANDS_( in, 0, 0
            case CMNDS::IN:
            printf( "-------------------------------------------------------------------\n"
                    "Please, enter number: " );

            if( scanf( "%lld", &arg ) != 1 )
            {
                printf( "SYNTAX ERROR in line: %zu\n"
                        "-------------------------------------------------------------------\n\n",
                        line );

                abort();
            }

            StackPush( &spu->stk, arg * MULTIPLYER );

            printf( "successfully pushed %lld \n"
                    "-------------------------------------------------------------------\n\n", arg ););