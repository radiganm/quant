// markowitz.cxx
// Mac Radigan
//
// Description:  This is a command line entry point for
//               demonstrating portfolio balancing using 
//               a basic applicaton Markowitz portfolio theory.
//
//               In the current implementation, a target return 
//               on investments is specified, short selling is 
//               permitted, and there is no inclusion of a risk-free
//               asset.
//
// See Also:     http://wikipedia.org/wiki/Modern_portfolio_theory
//               for more information on the Markowitz portfolio
//
//               http://finance.yahoo.com
//               for more information on Yahoo! Finance data sources
//

#include "quant.hxx"
#include "Portfolio.hxx"
#include "Series.hxx"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <streambuf>
#include <exception>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"

NS_QUANT_BEGIN
int usage(int argc, char* argv[], boost::program_options::options_description &desc) 
{
  std::cout << desc << std::endl;
  return 1;
}
NS_QUANT_END

using namespace std;
namespace po = boost::program_options;
using namespace boost::property_tree;
using namespace boost::program_options;
using namespace org::radigan::quant;
//struct sigcontext ctx;
using namespace log4cxx;
using namespace log4cxx::helpers;

LoggerPtr logger(Logger::getLogger("Markowitz"));


#if 0
//void bt_sighandler(int sig, struct sigcontext ctx) 
void bt_sighandler(int sig)
{
  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;
  if (sig == SIGSEGV)
  {
    printf("Got signal %d, faulty address is %p, " "from %p\n", sig, ctx.cr2, ctx.eip);
  } else {
    printf("Got signal %d\n", sig);
  }
  trace_size = backtrace(trace, 16);
  /* overwrite sigaction with caller's address */
  trace[1] = (void *)ctx.eip;
  messages = backtrace_symbols(trace, trace_size);
  /* skip first stack frame (points here) */
  printf("[bt] Execution path:\n");
  for (i=1; i<trace_size; ++i)
  {
    printf("[bt] #%d %s\n", i, messages[i]);
    char syscom[256];
    sprintf(syscom,"addr2line %p -e sighandler", trace[i]); //last parameter is the name of this app
    system(syscom);
  }
  exit(0);
}
#endif

void handler(int sig) {
  void *array[10];
  size_t size;
  // get void*'s for all entries on the stack
  size = backtrace(array, 10);
  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(int argc, char* argv[]) 
{
   signal(SIGSEGV, handler);
   /*
   struct sigaction sa;
   sa.sa_handler = (void *)bt_sighandler;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   sigaction(SIGSEGV, &sa, NULL);
   sigaction(SIGUSR1, &sa, NULL);
   */
   //
   int embedded;
   string filename;
   po::options_description desc("options");
   po::variables_map vm;
   po::notify(vm);
   desc.add_options()
     ("file,f", po::value<string>(), "input data file")
     ("help,h", "print this help message")
   ;
   po::store(po::parse_command_line(argc,argv,desc),vm);
   if(vm.count("help")) { return usage(argc,argv,desc); exit(0); }
   if(!vm.count("file")) { cerr << "no input file specified" << endl; exit(1); }
   int status = 1;
   try 
   {
     Portfolio* portfolio;
     ptree pt;
     read_xml(vm["file"].as<string>().c_str(), pt);
     string datapath = pt.get<string>("portfolio.database");
     string reportpath = pt.get<string>("portfolio.output");
     double roi = pt.get<double>("portfolio.roi");
     portfolio = new Portfolio(datapath);
     BOOST_FOREACH(const ptree::value_type &v, pt.get_child("portfolio.stocks")) 
     {
       string symbol = v.second.data();
       portfolio->addSeries(symbol);
     }
     portfolio->optimize(roi);
     cout << *portfolio << endl;
     portfolio->createReport(reportpath);
     status = 0;
   } catch(exception& e) {
      void *array[10];
      size_t size;
      size = backtrace(array, 10);
      backtrace_symbols_fd(array, size, STDERR_FILENO);
      exit(1);
     cerr << "exception: " << e.what() << endl;
     throw;
   }
   return status;
}

// *EOF*
