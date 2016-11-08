// 2016 Hiroyuki Ogasawara
// vim:ts=4 sw=4 noet:


class RealtimeLog {
	FILE*	LogFile;
	UI64	StartTime;
public:
	RealtimeLog();
	~RealtimeLog();
	void	Start( const char* log_file_name );
	void	Stop();
	void	Add( float w0, float w1, float w2, float w3, float wt );
	bool	IsLogging() const
	{
		return	LogFile != nullptr;
	}
};


