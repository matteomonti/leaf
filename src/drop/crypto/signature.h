// Forward declarations

namespace drop
{
    class signature;
    class signer;
    class verifier;
};

#if !defined(__forward__) && !defined(__drop__crypto__signature__h)
#define __drop__crypto__signature__h

// Libraries

#include <sodium.h>

// Includes

#include "drop/bytewise/bytewise.h"
#include "drop/data/tag.h"

namespace drop
{
    class signature : public tag <crypto_sign_BYTES>
    {
        // Friends

        friend class signer;

    public:

        // Nested classes

        class publickey : public tag <crypto_sign_PUBLICKEYBYTES>
        {
            // Friends

            friend class signer;
        };

        class secretkey : public tag <crypto_sign_SECRETKEYBYTES>
        {
            // Friends

            friend class signer;
        };
    };

    class signer
    {
    public:

        // Exceptions

        struct exceptions
        {
            class signature_failed : public std :: exception
            {
                const char * what() const throw();
            };
        };

    private:

        // Members

        signature :: publickey _publickey;
        signature :: secretkey _secretkey;

    public:

        // Constructors

        signer();
        signer(const signature :: publickey &, const signature :: secretkey &);

        // Getters

        const signature :: publickey & publickey() const;
        const signature :: secretkey & secretkey() const;

        // Methods

        signature sign(const buffer &);
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> * = nullptr> signature sign(const atypes & ...);
    };

    class verifier
    {
    public:

        // Exceptions

        struct exceptions
        {
            class verification_failed : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Members

        signature :: publickey _publickey;

    public:

        // Constructors

        verifier(const signature :: publickey &);

        // Getters

        const signature :: publickey & publickey() const;

        // Methods

        void verify(const signature &, const buffer &);
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <atypes> ()))> * = nullptr> void verify(const signature &, const atypes & ...);
    };
};

#endif
