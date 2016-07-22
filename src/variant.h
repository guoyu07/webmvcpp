#ifndef WEBMVCPP_VARIANTMAP_H
#define WEBMVCPP_VARIANTMAP_H

namespace webmvcpp
{
    class variant_impl : public webmvcobject
    {
        std::vector<unsigned char> valueBuffer;

        variant_impl(variant_impl&);
        variant_impl& operator=(variant_impl&);

    public:
        variant_impl(){}
        variant_impl(const std::vector<unsigned char> & val) { set(val); }
        variant_impl(const std::string & val) { set(val); }
        variant_impl(const std::wstring & val) { set(val); }
        variant_impl(const bool & val) { set(val); }
        variant_impl(const void * memPtr, unsigned int memSize) { set(memPtr, memSize); }
        template <typename T> variant_impl(const T & val) { set(val); }

        virtual ~variant_impl(){}

        variant_impl& operator=(const std::vector<unsigned char> & val)
        {
            valueBuffer = val;

            return *this;
        }

        variant_impl& operator=(const std::string & val)
        {
            set(val);

            return *this;
        }

        variant_impl& operator=(const std::wstring & val)
        {
            set(val);

            return *this;
        }

        variant_impl& operator=(const bool & val)
        {
            set(val);

            return *this;
        }

        std::string to_string() const
        { 
            if (valueBuffer.size()==0)
                return "<null>";

            return std::string((const char *)&valueBuffer.front(), valueBuffer.size()/sizeof(char)); 
        }
        std::wstring to_wstring() const
        { 
            if (valueBuffer.size()==0)
                return L"<null>";

            return std::wstring((const wchar_t *)&valueBuffer.front(), valueBuffer.size()/sizeof(wchar_t)); 
        }

        wchar_t to_wchar() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(wchar_t *)&valueBuffer.front(); 
        }

        double to_double() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(double *)&valueBuffer.front(); 
        }

        float to_float() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(float *)&valueBuffer.front();
        }

        long double long_double() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(long double *)&valueBuffer.front();
        }

        char to_char() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(char *)&valueBuffer.front(); 
        }

        unsigned char to_uchar() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(unsigned char *)&valueBuffer.front(); 
        }

        short to_short() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(short *)&valueBuffer.front();
        }

        unsigned short to_ushort() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(unsigned short *)&valueBuffer.front(); 
        }

        long to_long() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(long *)&valueBuffer.front(); 
        }

        unsigned long to_ulong() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(unsigned long *)&valueBuffer.front(); 
        }

        int to_int() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(int *)&valueBuffer.front(); 
        }

        unsigned int to_uint() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(unsigned int *)&valueBuffer.front(); 
        }

        long long to_longlong() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(long long *)&valueBuffer.front(); 
        }

        unsigned long long to_toulonglong() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(unsigned long long *)&valueBuffer.front(); 
        }

        int8_t to_int8() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(int8_t *)&valueBuffer.front(); 
        }

        uint8_t to_uint8() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(uint8_t *)&valueBuffer.front(); 
        }

        int16_t to_int16() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(int16_t *)&valueBuffer.front(); 
        }
        
        uint16_t to_uint16() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(uint16_t *)&valueBuffer.front(); 
        }
        
        int32_t to_int32() const
        { 
            if (valueBuffer.size()==0)
                return 0;

            return *(int32_t *)&valueBuffer.front(); 
        }
        
        uint32_t to_uint32() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(uint32_t *)&valueBuffer.front(); 
        }

        int64_t to_int64() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(int64_t *)&valueBuffer.front();
        }

        uint64_t to_uint64() const
        {
            if (valueBuffer.size()==0)
                return 0;

            return *(uint64_t *)&valueBuffer.front();
        }

        bool to_bool() const
        { 
            if (valueBuffer.size()==0)
                return false;

            return valueBuffer[0]!=0; 
        }

        const std::vector<unsigned char> & buffer() const
        {
            return valueBuffer;
        }

        void set(const std::string & val)
        {
            valueBuffer.resize(val.length());
            memcpy(&valueBuffer.front(), val.c_str(), val.length());
        }

        void set(const std::wstring & val)
        {
            valueBuffer.resize((val.length())*sizeof(wchar_t));
            memcpy(&valueBuffer.front(), val.c_str(), val.length()*sizeof(wchar_t));
        }

        void set(const bool & val)
        {
            valueBuffer.resize(1);
            valueBuffer[0] = val?1:0;
        }

        void set(const void * memPtr, unsigned int memSize)
        {
            valueBuffer.resize(memSize);
            memcpy(&valueBuffer.front(), memPtr, memSize);
        }

        template <typename T> void set(const T & val)
        {
            valueBuffer.resize(sizeof(T));
            memcpy(&valueBuffer.front(), &val, sizeof(T));
        }
    };

    struct variant_constructor {
        static variant_impl *make(){ return new variant_impl(); }
        static variant_impl *make(const std::vector<unsigned char> & val) { return new variant_impl(val); }
        static variant_impl *make(const std::string & val) { return new variant_impl(val); }
        static variant_impl *make(const std::wstring & val) { return new variant_impl(val); }
        static variant_impl *make(const bool & val) { return new variant_impl(val); }
        static variant_impl *make(const void * memPtr, unsigned int memSize) { return new variant_impl(memPtr, memSize); }
        template <typename T> static variant_impl *make(const T & val) { return new variant_impl(val); }
    };
    typedef boost::intrusive_ptr<variant_impl> variant;

    class variant_map : public std::map<std::string, variant>
    {
    public:
        variant operator[](const char *idx)
        {
            variant_map::const_iterator it = this->find(idx);
            if (it!=this->end())
                return it->second;
            else
            {
                const variant vptr = variant_constructor::make(uint64_t(0));
                this->insert(std::pair<std::string, variant>(idx, vptr));
                return vptr;
            }
        }

        const variant operator[](const char *idx) const
        {
            variant_map::const_iterator it = this->find(idx);
            if (it!=this->end())
                return it->second;
            else
            {
                return variant_constructor::make();
            }
        }
    };
}


#endif // WEBMVCPP_VARIANTMAP_H