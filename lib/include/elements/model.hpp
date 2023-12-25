/*=================================================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=================================================================================================*/
#if !defined(ELEMENTS_MODEL_DECEMBER_22_2023)
#define ELEMENTS_MODEL_DECEMBER_22_2023

#include <functional>
#include <infra/support.hpp>

namespace cycfi { namespace elements
{
   //==============================================================================================
   /** @class model
    *
    * The `model` class serves as an abstraction for a data type that is linked to one or more
    * user interface elements. The actual data is accessed and modified through the `get` and
    * `set` member functions of the derived class. A user interface element can be linked to a
    * `model` by supplying an `update_function` via the `on_update_ui(f)` member function.\n\n
    *
    * The conversion operator may be used to get a model's value via the derived class's `get`
    * member function. Example:
    * @code
    *    auto i = m; // m is an instance of a model with an `int` data type.
    * @endcode
    *
    * The assignment operator may be used to set the model's value via the derived class's `set`
    * member function. All linked user interface elements are updated when setting a new value.
    * Example:
    * @code
    *    m = 123; // m is an instance of a model with an `int` data type.
    * @endcode
    *
    * @tparam T The underlying type of the `model`.
    * @tparam Derived The class that derives from `model` that implements the value getter and
    *         setter.
    */
   //==============================================================================================
   template <typename T, typename Derived>
   class model
   {
   public:

      using value_type = T;
      using derived_type = Derived;
      using param_type = cycfi::param_type<value_type>;
      using update_param_type = cycfi::param_type<value_type>;
      using update_function = std::function<void(update_param_type)>;

      derived_type&           derived();
      derived_type const&     derived() const;

      model&                  operator=(model const&) = default;
      model&                  operator=(param_type val);
                              operator value_type() const;

      void                    update_ui();
      void                    update_ui(value_type val);
      void                    on_update_ui(update_function f);

   private:

      update_function         _update_ui;
   };

   //==============================================================================================
   /** @class value_model
    *
    * Class `value_model` is a derived class of `model` that handles the common case where the
    * data is held by value in the class itself. `get` and `set` member functions are provided
    * following the `model` requirements.
    *
    * @tparam T The underlying type of the `value_model`.
    */
   //==============================================================================================
   template <typename T>
   class value_model : public model<T, value_model<T>>
   {
   public:

      using base_type = model<T, value_model<T>>;
      using value_type = typename base_type::value_type;
      using param_type = typename base_type::param_type;
      using base_type::operator=;

                              value_model(param_type init = param_type{});

      value_type const&       get() const;
      void                    set(param_type val);

   private:

      value_type              _val;
   };

   //==============================================================================================
   /** @class reference_model
    *
    * Class `reference_model` is a derived class of `model` that handles the common case where the
    * data is referenced in the class. `get` and `set` member functions are provided following the
    * `model` requirements.
    *
    * @tparam T The underlying type of the `reference_model`.
    */
   //==============================================================================================
   template <typename T>
   class reference_model : public model<T, reference_model<T>>
   {
   public:

      using base_type = model<T, reference_model<T>>;
      using value_type = typename base_type::value_type;
      using param_type = typename base_type::param_type;
      using base_type::operator=;

                              reference_model(T& ref_);

      value_type const&       get() const;
      void                    set(param_type val);

   private:

      value_type&             _ref;
   };

   //==============================================================================================
   /** @class proxy_model
    *
    * Class `proxy_model` is a derived class of `model` that delegates the retrieval and
    * modification of data to another class specified by the `Delegate` template parameter. The
    * `get` and `set` member functions are undefined. It is the client's responsibility to define
    * these member functions, by providing specific specializations keyed by a supplied ID.\n\n
    *
    * The `proxy_model` holds a reference to a class that supplies the actual data. This reference
    * is provided by the client in its constructor. This is used by the `get` and `set`
    * implementations to extract and update specific data form the referenced class.
    *
    * @tparam T The underlying type of the `proxy_model`.
    * @tparam ID The ID used as specifier to implement specializations of `get` and `set` member
    *         functions.
    * @tparam Delegate Class that supplies the actual data.
    */
   //==============================================================================================
   template <typename T, typename ID, typename Delegate>
   class proxy_model : public model<T, proxy_model<T, ID, Delegate>>
   {
   public:

      using delegate_type = Delegate;
      using base_type = model<T, proxy_model<T, ID, delegate_type>>;
      using value_type = typename base_type::value_type;
      using param_type = typename base_type::param_type;
      using base_type::operator=;

                              proxy_model(delegate_type& ref_);

      value_type              get() const;
      void                    set(param_type val);

   private:

      delegate_type&          _ref;
   };

   //==============================================================================================
   // Inlines
   //==============================================================================================

   /** @brief Returns a reference to the derived class.
    */
   template <typename T, typename Derived>
   inline Derived& model<T, Derived>::derived()
   {
      return *static_cast<derived_type*>(this);
   }

   /** @brief Returns a const reference to the derived class.
    */
   template <typename T, typename Derived>
   inline Derived const& model<T, Derived>::derived() const
   {
      return *static_cast<derived_type const*>(this);
   }

   /** @brief Assign a new value to the model. This updates the model through the derived class's
    *         `set` member function and propagating the changes to all linked UI elements.
    * @param val The new value assigned to the model.
    */
   template <typename T, typename Derived>
   inline model<T, Derived>&
   model<T, Derived>::operator=(param_type val)
   {
      derived().set(val);
      update_ui(val);
      return *this;
   }

   /** @brief Gets the value of the model using the `get` member function of the derived class.
    */
   template <typename T, typename Derived>
   inline model<T, Derived>::operator value_type() const
   {
      return derived().get();
   }

   /** @brief Update all linked UI elements to the model's latest value.
    */
   template <typename T, typename Derived>
   inline void model<T, Derived>::update_ui()
   {
      update_ui(derived().get());
   }

   /** @brief Update all linked UI elements to the given `val`.
    *  @param val The new value used to update linked UI elements.
    */
   template <typename T, typename Derived>
   inline void model<T, Derived>::update_ui(value_type val)
   {
      if (_update_ui)
         _update_ui(val);
   }

   /**
    * @brief Set a function `f` to be invoked when a new value is set, enabling UI updates. This
    *        method can be called multiple times, and each supplied update function will be called
    *        sequentially at UI update time, in a first-come, first-served order.
    * @param f The update function.
    */
   template <typename T, typename Derived>
   inline void model<T, Derived>::on_update_ui(update_function f)
   {
      if (_update_ui)
      {
         // Chain call
         _update_ui =
            [prev_f = _update_ui, f](value_type val)
            {
               prev_f(val);
               f(val);
            };
      }
      else
      {
         _update_ui = f;
      }
   }

   /**
    * @brief Construct a `value_model` given optional initial value `init`
    * @param init Optional initial value.
    */
   template <typename T>
   inline value_model<T>::value_model(param_type init)
    : _val{init}
   {}

   /**
    * @brief Get the `value_model`'s value.
    */
   template <typename T>
   inline typename value_model<T>::value_type const&
   value_model<T>::get() const
   {
      return _val;
   }

   /**
    * @brief Set the value of the `value_model` to the specified `val`.
    * @param val The new value to assign to the model.
    */
   template <typename T>
   inline void value_model<T>::set(param_type val)
   {
      _val = val;
   }

   /**
    * @brief Construct a `reference_model` given a reference to a value used by the model.
    * @param ref_ A referece to the value used by the model.
    */
   template <typename T>
   inline reference_model<T>::reference_model(T& ref_)
    : _ref{ref_}
   {}

   /**
    * @brief Get the `reference_model`'s value.
    */
   template <typename T>
   inline typename reference_model<T>::value_type const&
   reference_model<T>::get() const
   {
      return _ref;
   }

   /**
    * @brief Set the value referenced by the `reference_model` to the specified `val`.
    * @param val The new value to assign to the model.
    */
   template <typename T>
   inline void reference_model<T>::set(param_type val)
   {
      _ref = val;
   }

   /**
    * @brief Construct a `proxy_model` given a reference to the target class `Delegate`.
    * @param ref_ A referece to the to the target class.
    */
   template <typename T, typename ID, typename Delegate>
   inline proxy_model<T, ID, Delegate>::proxy_model(Delegate& ref_)
    : _ref{ref_}
   {}
}}

#endif
