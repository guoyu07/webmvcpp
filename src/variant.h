#ifndef WEBMVCPP_VARIANTMAP_H
#define WEBMVCPP_VARIANTMAP_H

namespace webmvcpp
{
    class variant_impl
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

		template <typename T> variant_impl& operator=(const T & val)
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

		unsigned long reference = 0;
    };

	class variant
	{
	public:

		typedef variant_impl element_type;

		variant() : p_(0)
		{
		}

		inline void variant_add_ref(variant_impl *p)
		{
			++(p->reference);
		}

		inline void variant_release(variant_impl *p)
		{
			if (--(p->reference) == 0)
				delete p;
		}

		variant(variant_impl * p, bool add_ref = true) : p_(p)
		{
			if (p_ != 0 && add_ref) variant_add_ref(p_);
		}

		variant(variant const & rhs) : p_(rhs.p_)
		{
			if (p_ != 0) variant_add_ref(p_);
		}

		~variant()
		{
			if (p_ != 0) variant_release(p_);
		}

		variant & operator=(variant const & rhs)
		{
			variant(rhs).swap(*this);
			return *this;
		}

		variant & operator=(variant_impl * rhs)
		{
			variant(rhs).swap(*this);
			return *this;
		}

		variant& operator=(const std::vector<unsigned char> & val)
		{
			p_->set(val);

			return *this;
		}

		variant& operator=(const std::string & val)
		{
			p_->set(val);

			return *this;
		}

		variant& operator=(const std::wstring & val)
		{
			p_->set(val);

			return *this;
		}

		variant& operator=(const bool & val)
		{
			p_->set(val);

			return *this;
		}

		template <typename T> variant& operator=(const T & val)
		{
			p_->set(val);

			return *this;
		}

		variant_impl * get() const
		{
			return p_;
		}

		variant_impl & operator*() const
		{
			return *p_;
		}

		variant_impl * operator->() const
		{
			return p_;
		}

		void swap(variant & rhs)
		{
			variant_impl * tmp = p_;
			p_ = rhs.p_;
			rhs.p_ = tmp;
		}

	private:
		variant_impl * p_;
	};

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
                variant vptr = new variant_impl(0);
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
                return new variant_impl();
            }
        }
    };
}


#endif // WEBMVCPP_VARIANTMAP_H