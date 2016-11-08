// 2016 Hiroyuki Ogasawara
// vim:ts=4 sw=4 noet:

#include	"flCore.h"
#include	<stdio.h>
#include	"RealtimeLog.h"



RealtimeLog::RealtimeLog() :
	LogFile( nullptr )
{
}


RealtimeLog::~RealtimeLog()
{
	Stop();
}


void	RealtimeLog::Start( const char* log_file_name )
{
	StartTime= GetTickCount();
	const int	BUFFER_SIZE= 1024*4;
	DWORD	attr= GetFileAttributes( log_file_name );
	if( attr != INVALID_FILE_ATTRIBUTES ){
		char	new_file_name[BUFFER_SIZE];
		strcpy( new_file_name, log_file_name );
		strcat( new_file_name, ".backup" );
		DWORD	backup_attr= GetFileAttributes( new_file_name );
		if( backup_attr != INVALID_FILE_ATTRIBUTES ){
			DeleteFile( new_file_name );
		}
		MoveFile( log_file_name, new_file_name );
	}
	LogFile= fopen( log_file_name, "w" );
}


void	RealtimeLog::Stop()
{
	if( LogFile ){
		fclose( LogFile );
		LogFile= nullptr;
	}
}


void	RealtimeLog::Add( float w0, float w1, float w2, float w3, float wt )
{
	if( IsLogging() ){
		UI64	timestamp= GetTickCount() - StartTime;
		fprintf( LogFile, "%I64d, %f, %f, %f, %f, %f\n", timestamp, w0, w1, w2, w3, wt );
	}
}


