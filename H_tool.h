////////////////////////////////////////////////////////////////
//
//  H_tool library
//
//  author(s):
//  ENDESGA - https://x.com/ENDESGA | https://bsky.app/profile/endesga.bsky.social
//
//  https://github.com/H-language/H_tool
//  2025 - CC0 - FOSS forever
//

////////////////////////////////
/// include(s)

#include <H.h>

////////////////////////////////
/// version

#define H_TOOL_VERSION_MAJOR 0
#define H_TOOL_VERSION_MINOR 1
#define H_TOOL_VERSION_PATCH 0
#define H_TOOL_VERSION AS_BYTES( H_TOOL_VERSION_MAJOR ) "." AS_BYTES( H_TOOL_VERSION_MINOR ) "." AS_BYTES( H_TOOL_VERSION_PATCH )

////////////////////////////////

#define system_tool_exists( NAME ) ( system( PICK( OS_LINUX, "command -v " #NAME " >/dev/null", "where " #NAME " >nul" ) " 2>&1" ) is 0 )

////////////////////////////////
/// messages

#define max_message_parts_count 4
#define max_message_part_size 128
global byte message_parts[ max_message_parts_count ][ max_message_part_size ];
global n1 message_parts_count = 0;

#define message_parts_add( BYTES ) bytes_paste( message_parts[ message_parts_count++ ], BYTES )

////////////////////////////////
/// user input

#define max_inputs_count 16
#define max_input_size 32
global byte inputs[ max_inputs_count ][ max_input_size ];
global n1 inputs_count = 0;
global n1 current_input = 0;

#define inputs_add( BYTES ) bytes_paste( inputs[ inputs_count++ ], BYTES )

#define inputs_collect_start_parameters()\
	if( start_parameters_count > 1 )\
	{\
		temp n1 parameter = 1;\
		while( parameter < start_parameters_count )\
		{\
			inputs_add( start_parameters[ parameter++ ] );\
		}\
	}

////////
// input handling

#define input_next()\
	START_DEF\
	{\
		++current_input;\
		jump process_input;\
	}\
	END_DEF

#define check_input( NAME, BYTES, BYTES_SIZE )\
	START_DEF\
	{\
		if( inputs[ current_input ][ 0 ] isnt '\0' and bytes_compare( inputs[ current_input ], BYTES, BYTES_SIZE + 1 ) )\
		{\
			message_parts_count = 0;\
			message_parts_add( format_magenta "did you mean `" format_yellow );\
			message_parts_add( BYTES );\
			message_parts_add( format_magenta "`?" );\
			input_next();\
		}\
	}\
	END_DEF

#define H_tool_start()\
	process_input:\
	print_clear()

fn get_inputs()
{
	if( inputs[ current_input ][ 0 ] is '\0' )
	{
		iter( i, inputs_count )
		{
			bytes_clear( inputs[ i ], max_input_size );
		}
		inputs_count = 0;
		current_input = 0;

		temp const byte ref print_input = get_os_input();
		perm byte input[ max_input_size ];
		temp byte ref input_ref = input;

		process_print_input:
		{
			temp byte input_byte = val_of( print_input );
			with( input_byte )
			{
				when( '\0', ' ', '\t', '\r', '\n' )
				{
					if( input_ref isnt input )
					{
						bytes_end( input_ref );
						inputs_add( input );
						input_ref = input;
					}

					if( input_byte is '\0' )
					{
						skip;
					}

					++print_input;
					jump process_print_input;
				}

				//

				when( '"' )
				{
					++print_input;
					process_input_quotes:
					{
						input_byte = val_of( print_input );
						with( input_byte )
						{
							when( '"' )
							{
								++print_input;
								jump process_print_input;
							}

							//

							other
							{
								bytes_set_move( input_ref, input_byte );
								++print_input;
								jump process_input_quotes;
							}
						}
					}
				}

				//

				other
				{
					bytes_set_move( input_ref, input_byte );
					++print_input;
					jump process_print_input;
				}
			}
		}
	}
}

////////////////////////////////
/// text user interface

////////
// messages

#define print_message()\
	if( message_parts_count isnt 0 )\
	{\
		iter( message_part, message_parts_count )\
		{\
			print( message_parts[ message_part ] );\
			bytes_clear( message_parts[ message_part ], max_message_part_size );\
		}\
		message_parts_count = 0;\
		print_newline();\
	}

////////////////////////////////////////////////////////////////
