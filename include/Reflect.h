#ifndef _HELLEBORE_CONSULTING_LLC_REFLECTION_H_
#define _HELLEBORE_CONSULTING_LLC_REFLECTION_H_

// www.helleboreconsulting.com

#include <stdint.h>
#include <ostream>
#include <type_traits>
#include <string>
#include <memory>

///
/// \def HELLEBORE_REFLECTION_BUILDER
///
/// This macro helps build reflection variant classes to support POD types.
/// This was specially adapted to provide for output to std::string.
///
#define HELLEBORE_REFLECTION_BUILDER(RTYPE) protected: class Reflect_##RTYPE : public TypeBase \
{ \
	public: \
		Reflect_##RTYPE() : TypeBase(), bound(nullptr) {} \
		Reflect_##RTYPE(const Reflect_##RTYPE& other) : TypeBase(other) {}  \
		Reflect_##RTYPE(Reflect_##RTYPE&& other) : TypeBase(other) { std::swap(this->bound, other.bound); }  \
		Reflect_##RTYPE(RTYPE* const x) : TypeBase(), bound(x) {} \
		virtual ~Reflect_##RTYPE() {} \
		virtual std::string string() { return std::to_string(*this->bound); } \
		virtual void set(void* x) { *bound = *(reinterpret_cast<const RTYPE*>(x)); } \
		virtual void bind(void* x) { this->bound = reinterpret_cast<RTYPE*>(x); } \
		virtual void* get() const { return bound; } \
		virtual const std::type_info& type() const { return typeid(RTYPE); } \
		virtual bool operator<(const TypeBase& other) const { auto otherTypePtr = reinterpret_cast<const Reflect_##RTYPE*>(&other); return *(this->bound) < *(otherTypePtr->bound); } \
	private: \
		RTYPE* bound; \
}; public: \
Reflect(RTYPE* x) : value(std::make_shared<Reflect_##RTYPE>(x)) {} \
operator RTYPE() const { return *(reinterpret_cast<RTYPE*>(this->value->get())); }


///
/// \class Reflect
///
/// \author	John Farrier
///
/// \brief	Reflect Type Implementation
///
/// This class can be used to reflect types in a generic way.
///
class Reflect
{
	protected:
		///
		/// \class TypeBase
		///
		/// \author	John Farrier
		///
		class TypeBase
		{
			public:
				TypeBase() { }
				TypeBase(const TypeBase&) { }
				TypeBase(TypeBase&&) { }
				virtual ~TypeBase() { }
				virtual std::string string() { std::string empty; return empty; }
				virtual void set(void*) { }
				virtual void bind(void*) { }
				virtual void* get() const { return nullptr; }
				virtual const std::type_info& type() const { return typeid(TypeBase); }
				virtual bool operator<(const TypeBase& other) const { return false; }
		};

	public:
		///
		/// Default constructor
		///
		Reflect() : value(nullptr) {}

		///
		/// Copy constructor
		///
		Reflect(const Reflect& other) : value(other.value) {}

		///
		/// Move constructor
		///
		Reflect(Reflect&& other) : value(nullptr) { std::swap(this->value, other.value); }

		///
		/// Macro to build a class to hold uint8_t
		///
		HELLEBORE_REFLECTION_BUILDER(uint8_t);
		HELLEBORE_REFLECTION_BUILDER(uint16_t);
		HELLEBORE_REFLECTION_BUILDER(uint32_t);
		HELLEBORE_REFLECTION_BUILDER(uint64_t);

		HELLEBORE_REFLECTION_BUILDER(int8_t);
		HELLEBORE_REFLECTION_BUILDER(int16_t);
		HELLEBORE_REFLECTION_BUILDER(int32_t);
		HELLEBORE_REFLECTION_BUILDER(int64_t);

		HELLEBORE_REFLECTION_BUILDER(float);
		HELLEBORE_REFLECTION_BUILDER(double);
		HELLEBORE_REFLECTION_BUILDER(long);
		HELLEBORE_REFLECTION_BUILDER(bool);
		HELLEBORE_REFLECTION_BUILDER(char);

		Reflect(std::string* x) : value(std::make_shared<Reflect_string>(x)) {} 
		Reflect(std::string& x) : value(std::make_shared<Reflect_string>(x)) {} 
			
		///
		/// Trivial destructor
		///
		virtual ~Reflect() { }

		///
		/// Sets the value of the variant type held.
		///
		template<typename T> T& operator=(T& x)
		{
			T* ptr = &x;
			this->value->set(reinterpret_cast<void*>(ptr));
			return *(reinterpret_cast<T*>(this->value->get()));
		}

		///
		/// Binds to the location in memory of a variant type (as opposed to holding a copy.)
		///
		template<typename T> T& operator=(T* const x)
		{
			this->value->bind(reinterpret_cast<void*>(x));
			return *(reinterpret_cast<T*>(this->value->get()));
		}

		///
		/// String conversion operator
		///
		operator std::string const()
		{
			std::string str;

			if(this->value != nullptr)
			{
				str = this->value->string();
			}

			return str;
		}

		///
		/// Returns the RTTI type of the variant type held.
		///
		const std::type_info& type() const { return this->value->type(); }

		///
		/// Allows for ostream output of this variant type.
		///
		friend std::ostream& operator<< (std::ostream& os, const Reflect& v)
		{
			os << v.value->string();
			return os;
		}

		///
		/// Equality overload
		///
		bool operator==(const std::string& x)
		{
			return x == (this->value->string());
		}

		///
		/// Support < comparisons for STL containers.
		///
		bool operator<(const Reflect& other) const
		{
			// Try to compare values if the types are the same.
			if(this->type() == other.type())
			{
				return (*this->value < *other.value);
			}

			// Compare types as a backup.
			return this->type().before(other.type());
		}

	protected:
		///
		/// \class Reflect_string
		///
		/// \author	John Farrier
		///
		/// Reflect type implementation for a non-POD type
		///
		class Reflect_string : public TypeBase
		{
			public: 
				Reflect_string() : TypeBase() {} 
				Reflect_string(const Reflect_string& other) : TypeBase(other) {}  
				Reflect_string(std::string& x) : TypeBase(), bound(&x) {} 
				Reflect_string(std::string* const x) : TypeBase(), bound(x) {} 
				virtual ~Reflect_string() {} 
				virtual std::string string() { return *reinterpret_cast<const std::string*>(this->bound); } 
				virtual void set(void* x) { *reinterpret_cast<std::string*>(bound) = *(reinterpret_cast<const std::string*>(x)); } 
				virtual void bind(void* x) { this->bound = reinterpret_cast<std::string*>(x); } 
				virtual void* get() const { return bound; } 
				virtual const std::type_info& type() const { return typeid(std::string); } 

			private:
				std::string* bound;
		}; 

		/// A shared pointer to the value held by this reflection variant.
		std::shared_ptr<TypeBase> value;
};

#endif
