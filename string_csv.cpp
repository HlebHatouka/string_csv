#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include <ctime>
#include <algorithm>
#include <exception>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
using boost::multi_index_container;
using namespace boost::multi_index;

struct StringCSV
{
    StringCSV(const uint64_t &_id,
            const std::string &_partner,
            const uint32_t &_number,
            const time_t &_date,
            const bool &_status
            );

    uint64_t id;
    std::string partner;
    uint32_t number;
    time_t date;
    bool status;
    
    friend std::ostream& operator<<(std::ostream &stream, const StringCSV &str);
    bool operator<(const StringCSV &str)const
    {
        return id < str.id;
    }
};

struct id       {};
struct partner  {};
struct number   {};
struct date     {};
struct status   {};

typedef multi_index_container<
    StringCSV,
        indexed_by<
            ordered_unique<
                tag<id>, BOOST_MULTI_INDEX_MEMBER(StringCSV, uint64_t, id)>,
            ordered_unique<
                tag<partner>, BOOST_MULTI_INDEX_MEMBER(StringCSV, std::string, partner)>,
            ordered_non_unique<
                tag<number>, BOOST_MULTI_INDEX_MEMBER(StringCSV, uint32_t, number)>,
            ordered_non_unique<
                tag<date>, BOOST_MULTI_INDEX_MEMBER(StringCSV, time_t, date)>,
            ordered_non_unique<
                tag<status>, BOOST_MULTI_INDEX_MEMBER(StringCSV, bool, status)> 
        >
> StringCSV_set;

bool csv_file_exists(const fs::path &file_path);

void read_data(const std::string &path, StringCSV_set &csv_strings);

template<typename Tag, typename MultiIndexContainer>
void print_by(const MultiIndexContainer &container);

template<typename Tag, typename ValueType, typename MultiIndexContainer>
void print_if(const MultiIndexContainer &container, const ValueType &value);
                
template<typename Tag, typename ValueType, typename MultiIndexContainer>
void print_if_between(const MultiIndexContainer &container,
                        const ValueType &first, const ValueType &second); 

template<typename Tag, typename MultiIndexContainer>
void print_max(const MultiIndexContainer &container);

int main(int argc, char *argv[])
{
    if ( argc != 2)
        throw "should be 1 argument as csv file";

    try
    {
        if(csv_file_exists(argv[1]))
        { 
            StringCSV_set csv_strings;

            read_data(argv[1], csv_strings);

            print_by<id>(csv_strings);
            std::cout << std::endl;

            print_max<id>(csv_strings);
            std::cout << std::endl;

            print_if<id, uint64_t>(csv_strings, 100);
            std::cout << std::endl;

            time_t start2019 = 1546300801;
            time_t end2019 = 1577836799;
            print_if_between<date, time_t>(csv_strings,start2019, end2019);

        }
        else
            throw "csv file does not exist";  
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    catch (const char *e)
    {
        std::cerr << e << std::endl;
        exit(1);
    }
    return 0;
}

StringCSV::StringCSV(const uint64_t &_id,
                        const std::string &_partner,
                        const uint32_t &_number,
                        const time_t &_date,
                        const bool &_status 
                    ) :
            id(_id), partner(_partner),
            number(_number), date(_date), 
            status(_status)
            {}

std::ostream& operator<<(std::ostream &stream, const StringCSV &str)
{
    stream << str.id << " " << str.partner << " "
            << " " << str.number << " " << str.date
            << " " << str.status << std::endl;
    return stream;
}

bool csv_file_exists( const fs::path &file_path)
{
    return file_path.has_extension() && 
            file_path.extension().string() == ".csv" &&
            fs::exists(file_path);
}

void read_data(const std::string &path, 
                StringCSV_set &csv_strings)
{
    std::ifstream fin( path);
    if (!fin.is_open())
        throw "can not open the file";
            
    uint64_t id;
    std::string partner;
    uint32_t number;
    time_t date;
    bool status;
            
    std::string line;
    std::string data[5];

    while (fin.good())
    {
        while (std::getline(fin, line))
        {
            int i = 0;
            std::istringstream line_stream(line);
            while (std::getline(line_stream, data[i], ';'))
            {
                std::istringstream word_stream(data[i]);
                switch (i)
                {
                    case 0:
                        word_stream >> id;
                        break;
                    case 1:
                        word_stream >> partner;
                        break;
                    case 2:
                        word_stream >> number;
                        break;
                    case 3:
                        word_stream >> date;
                        break;
                    case 4:
                        word_stream >> status;
                        break;
                }
                ++i;
            }
            csv_strings.insert(StringCSV(id, partner, 
                                    number, date, status));
        }
    }
    fin.close();
}

template<typename Tag, typename MultiIndexContainer>
void print_by(const MultiIndexContainer &container)
{
    const auto &name_index = get<Tag>(container);
    typedef typename MultiIndexContainer::value_type value_type;
    std::copy(name_index.begin(), name_index.end(),
                std::ostream_iterator<value_type>(std::cout));
}

template<typename Tag, typename ValueType, typename MultiIndexContainer>
void print_if(const MultiIndexContainer &container, const ValueType &value)
{
    const auto index_tie = get<Tag>(container).equal_range(value);
    typedef typename MultiIndexContainer::value_type value_type;
    std::copy(index_tie.first, index_tie.second, 
                std::ostream_iterator<value_type>(std::cout));
}

template<typename Tag, typename ValueType, typename MultiIndexContainer>
void print_if_between(const MultiIndexContainer &container,
                        const ValueType &first, const ValueType &second)
{
    auto lower = get<Tag>(container).lower_bound(first);
    auto upper = get<Tag>(container).upper_bound(second);

    typedef typename MultiIndexContainer::value_type value_type;
    std::copy(lower, upper, 
                std::ostream_iterator<value_type>(std::cout));
}

template<typename Tag, typename MultiIndexContainer>
void print_max(const MultiIndexContainer &container)
{
    const auto &name_index = get<Tag>(container);
    auto element = std::max_element(name_index.begin(), name_index.end());
    std::cout << *element;
}
