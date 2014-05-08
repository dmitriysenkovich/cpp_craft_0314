#include "aggregator.h"

namespace multicast_communication
{
	aggregator::aggregator(market_data_processor_derived& md) : md(md)
	{
		end = false;
	}

	void aggregator::SaveOne(std::string new_data, message_type type)
	{
		if (type == message_type::QUOTE)
			quote_data.push(new_data);
		else
			trade_data.push(new_data);
	}

	void aggregator::StartOutput()
	{
		boost::thread_group threads;

		threads.create_thread(boost::bind(&aggregator::QuoteOutput, this));
		threads.create_thread(boost::bind(&aggregator::TradeOutput, this));
		//threads.join_all();
	}

	void aggregator::StopOutput()
	{
		end = true;
	}

	void aggregator::QuoteOutput()
	{
		converter conv;
		while (!end || !quote_data.IsEmpty())
		{
			if (!quote_data.IsEmpty())
			{
				std::string tmp = quote_data.pop();
				conv.ConvertToQuote(tmp, boost::bind(&market_data_processor_derived::new_quote, &md, _1));
			}
			else
				boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}

	void aggregator::TradeOutput()
	{
		converter conv;
		while (!end || !trade_data.IsEmpty())
		{
			if (!trade_data.IsEmpty())
				conv.ConvertToTrade(trade_data.pop(), boost::bind(&market_data_processor_derived::new_trade, &md, _1));
			else
				boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}
}
