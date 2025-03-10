//
//  POSLensCore.h
//  POSLens
//
//  Created by Pavel Osipov on 06/02/2018.
//  Copyright © 2018 Pavel Osipov. All rights reserved.
//

#import "POSLensValue.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#   import <ReactiveObjC/ReactiveObjC.h>
#pragma clang diagnostic pop

#import <POSErrorHandling/POSErrorHandling.h>

NS_ASSUME_NONNULL_BEGIN

///
/// Information object with state of performed update.
///
@interface POSLensValueUpdate<__covariant ValueType:POSLensValue *> : NSObject

@property (nonatomic, readonly, nullable) ValueType oldValue;
@property (nonatomic, readonly, nullable) ValueType actualValue;

POS_INIT_UNAVAILABLE

@end

///
/// Provides read-only access for some part of the object.
///
@interface POSLens<__covariant ValueType:POSLensValue *> : NSObject

///
/// @brief      Actual instance of POSLensValue object.
///
/// @discussion Value is resolved lazy on demand. It is possible to have POSLens object
///             which points to nonexisting object when created. Even in that case, you'll
///             receive notification after value addition.
///
@property (nonatomic, readonly, nullable) ValueType value;

///
/// @brief      Signal which emits actual values.
///             It notifies about property addition, update, and removal.
///
/// @discussion Updates propagate recursively from children to parents. Update of the child
///             object means that its parent is updated as well. The opposite statement is not
///             true in general. For example, let's consider that we have root object Root.
///             Root has two properties A and B. Each property has two children A1, A2 and B1 and
///             B2 correspondently. When B2 property instance is added, updated or removed then
///             signals of B2, B, and Root objects will emit actual values. At the same time
///             the signals of A1, A2, and A objects will be quiet despite that their parent was updated.
///
///             If the property doesn't exist or it was removed, then the signal
///             will emit nil or default value if exist.
///
/// @remarks    Signal emits actual POSLensValue on subscription.
///
///             Signal emits values on the updating thread.
///
@property (nonatomic, readonly) RACSignal<POSLensValueUpdate<ValueType> *> *historicalValueUpdates;

///
/// @brief      Signal which emits actual values.
///
/// @discussion Signal emits values at the same time as historicalValueUpdates does, but contains only new values.
///
@property (nonatomic, readonly) RACSignal<ValueType> *valueUpdates;

///
/// @brief      Fluent version of `lensForKey:` method which retrieves lens for underlying property
///             without default value specification.
///             @code lens[account.email][@"credentials"]["accessToken"] @endcode
///
/// @remarks    Use that method only if there is no default value for accessing property,
///             or you are not going to modify its underlying properties.
///
- (POSLens *)objectForKeyedSubscript:(NSString *)key;

///
/// @brief      Method for retrieving lens without default value specification.
///
/// @remarks    Use that method only if there is no default value for accessing object's property,
///             or you are not going to modify its subgraph.
///
- (POSLens *)lensForKey:(NSString *)key;

///
/// @brief      Method for retrieving lens with default value specification.
///
/// @discussion Lens object would insert the default value into the underlying object only
///             if some of its properties were assigned to nondefault values.
///
- (POSLens *)lensForKey:(NSString *)key defaultValue:(nullable POSLensValue *)defaultValue;

///
/// @brief      Method for retrieving lens with specified keypath.
///
/// @remarks    Use that method only if there is no default value for accessing object's property,
///             or you are not going to modify its subgraph.
///
- (POSLens *)lensForKeyPath:(NSString *)keyPath;

POS_INIT_UNAVAILABLE

@end

#pragma mark -

///
/// Provides read-write access for the some part of the object's data structure.
///
@interface POSMutableLens<__covariant ValueType:POSLensValue *> : POSLens<ValueType>

///
/// @brief      Fluent version of `lensForKey:` method which retrieves lens for underlying property
///             without default value specification.
///             @code lens[account.email][@"credentials"]["accessToken"] @endcode
///
/// @remark     Use that method only if there is no default value for accessing property,
///             or you are not going to modify its underlying properties.
///
- (POSMutableLens *)objectForKeyedSubscript:(NSString *)key;

///
/// @brief      Method for retrieving lens without default value specification.
///
/// @remark     Use that method only if there is no default value for accessing object's property,
///             or you are not going to modify its subgraph.
///
- (POSMutableLens *)lensForKey:(NSString *)key;

///
/// @brief      Method for retrieving lens with specified keypath.
///
/// @remarks    Use that method only if there is no default value for accessing object's property,
///             or you are not going to modify its subgraph.
///
- (POSMutableLens *)lensForKeyPath:(NSString *)keyPath;

///
/// @brief      Method for retrieving lens with default value specification.
///
/// @discussion Lens object would insert the default value into the underlying object only
///             if some of its properties were assigned to nondefault values.
///
- (POSMutableLens *)lensForKey:(NSString *)key defaultValue:(nullable POSLensValue *)defaultValue;

///
/// @brief      Reloads underlying POSLenValue from value store.
///
/// @discussion Returns NO and `error` out parameter if value store is in trouble to load it.
///
/// @returns    YES if the value was successfully loaded from the store.
///
- (BOOL)resetValue:(NSError **)error;

///
/// @brief      Atomically updates underlying POSLenValue.
///
/// @remarks    The method always updates underlying value in memory.
///             In a case of persisting failure, the error will be logged and ignored.
///
- (void)forceUpdateValue:(nullable ValueType)value;

///
/// @brief      Atomically updates underlying POSLenValue.
///
/// @discussion The method returns NO and `error` out parameter in the following cases:
///             (a) there are neither parent object or the default value for it,
///             (b) underlying value store is in trouble to persist it.
///
/// @returns    YES if the updated value was successfully persisted in the store.
///
- (BOOL)updateValue:(nullable ValueType)value error:(NSError **)error;

///
/// @brief      Atomically updates underlying property with specified key.
///
/// @remarks    The method always updates underlying property with specified key in memory.
///             In a case of persisting failure, the error will be logged and ignored.
///
/// @discussion The difference between that method and updateValue:atKeyPath: is that
///             key used as is without splitting it into components using dots symbols.
///             The good example of such key is user's email – user@example.co.uk.
///
- (void)forceUpdateValue:(nullable POSLensValue *)value atKey:(NSString *)key;

///
/// @brief      Atomically updates underlying property with specified key.
///
/// @discussion The method returns NO and `error` out parameter in the following cases:
///             (a) there are neither parent object or the default value for it,
///             (b) underlying value store is in trouble to persist it.
///
///             The difference between that method and updateValue:atKeyPath: is that
///             key used as is without splitting it into components using dots symbols.
///             The good example of such key is user's email – user@example.co.uk.
///
/// @returns    YES if the updated value was successfully persisted in the store.
///
- (BOOL)updateValue:(nullable POSLensValue *)value atKey:(NSString *)key error:(NSError **)error;

///
/// @brief      Atomically updates underlying property with specified keypath.
///
/// @remarks    The method always updates underlying property with specified keypath in memory.
///             In a case of persisting failure, the error will be logged and ignored.
///
/// @discussion The difference between that method and updateValue:atKey: is that
///             keypath will be split into keys using dots symbols. So the value at
///             keypath user@example.co.uk will be updated using three levels deeper lens:
///             'user@example', 'co', 'uk'.
///
- (void)forceUpdateValue:(nullable POSLensValue *)value atKeyPath:(NSString *)keyPath;

///
/// @brief      Atomically updates underlying property with specified keypath.
///
/// @discussion The method returns NO and `error` out parameter in the following cases:
///             (a) there are neither parent object or the default value for it,
///             (b) underlying value store is in trouble to persist it.
///
///             The difference between that method and updateValue:atKey: is that
///             keypath will be split into keys using dots symbols. So the value at
///             keypath user@example.co.uk will be updated using three levels deeper lens:
///             'user@example', 'co', 'uk'.
///
/// @returns    YES if the updated value was successfully persisted in the store.
///
- (BOOL)updateValue:(nullable POSLensValue *)value atKeyPath:(NSString *)keyPath error:(NSError **)error;

///
/// @brief      Atomically replaces underlying POSLensValue with a new instance created by
///             the thread-safe updateBlock.
///
/// @discussion Update block provides current value as an input parameter for exclusive modification.
///             Other value clients cannot read and modify it until block execution is in progress.
///             This statement is true even if those clients use independent instances of the POSLens
///             objects to manage the underlying value at the same path in the object's graph.
///
///             The most straightforward use-case for that method is incrementing some counter in a thread-safe manner.
///
/// @remarks    The method updates the underlying value in memory except for the case when block's error
///             out parameter is not nil. When underlying persisting store failed to save the value,
///             its error will be logged and ignored.
///
- (void)forceUpdateValueWithBlock:(ValueType _Nullable (^)(ValueType _Nullable oldValue, NSError **error))updateBlock;

///
/// @brief      Atomically replaces underlying POSLensValue with a new instance created by
///             the thread-safe updateBlock.
///
/// @discussion Update block provides current value as an input parameter for exclusive modification.
///             Other value clients cannot read and modify it until block execution is in progress.
///             This statement is true even if those clients use independent instances of the POSLens
///             objects to manage the underlying value at the same path in the object's graph.
///
///             The most straightforward use-case for that method is incrementing some counter in a thread-safe manner.
///
///             The method returns NO and `error` out parameter in the following cases:
///             (a) there are neither parent object or the default value for it,
///             (b) underlying value store is in trouble to persist it.
///
/// @returns    YES if the updated value was successfully persisted in the store.
///
- (BOOL)updateValueWithBlock:(ValueType _Nullable (^)(ValueType _Nullable oldValue, NSError **error))updateBlock
                       error:(NSError **)error;

///
/// @brief      Atomically property with specified key with a new instance created by
///             the thread-safe updateBlock.
///
/// @discussion Update block provides current value as an input parameter for exclusive modification.
///             Other value clients cannot read and modify it until block execution is in progress.
///             This statement is true even if those clients use independent instances of the POSLens
///             objects to manage the underlying value at the same path in the object's graph.
///
///             The most straightforward use-case for that method is incrementing some counter in a thread-safe manner.
///
/// @remarks    The method updates the property with specified key in memory except for the case when block's error
///             out parameter is not nil. When underlying persisting store failed to save the value,
///             its error will be logged and ignored.
///
- (void)forceUpdateValueAtKey:(NSString *)key
                    withBlock:(id _Nullable (^)(id oldValue, NSError **error))block;

///
/// @brief      Atomically property with specified key with a new instance created by
///             the thread-safe updateBlock.
///
/// @discussion Update block provides current value as an input parameter for exclusive modification.
///             Other value clients cannot read and modify it until block execution is in progress.
///             This statement is true even if those clients use independent instances of the POSLens
///             objects to manage the underlying value at the same path in the object's graph.
///
///             The most straightforward use-case for that method is incrementing some counter in a thread-safe manner.
///
///             The method returns NO and `error` out parameter in the following cases:
///             (a) there are neither parent object or the default value for it,
///             (b) underlying value store is in trouble to persist it.
///
/// @returns    YES if the value was successfully persisted in the store.
///
- (BOOL)updateValueAtKey:(NSString *)key
               withBlock:(id _Nullable (^)(id _Nullable oldValue, NSError **error))block
                   error:(NSError **)error;

///
/// @brief      Atomically property with specified keypath with a new instance created by
///             the thread-safe updateBlock.
///
/// @discussion Update block provides current value as an input parameter for exclusive modification.
///             Other value clients cannot read and modify it until block execution is in progress.
///             This statement is true even if those clients use independent instances of the POSLens
///             objects to manage the underlying value at the same path in the object's graph.
///
///             The most straightforward use-case for that method is incrementing some counter in a thread-safe manner.
///
/// @remarks    The method updates the property with specified keypath in memory except for the case when block's error
///             out parameter is not nil. When underlying persisting store failed to save the value,
///             its error will be logged and ignored.
///
- (void)forceUpdateValueAtKeyPath:(NSString *)keyPath
                        withBlock:(id _Nullable (^)(id _Nullable oldValue, NSError **error))block;

///
/// @brief      Atomically property with specified keypath with a new instance created by
///             the thread-safe updateBlock.
///
/// @discussion Update block provides current value as an input parameter for exclusive modification.
///             Other value clients cannot read and modify it until block execution is in progress.
///             This statement is true even if those clients use independent instances of the POSLens
///             objects to manage the underlying value at the same path in the object's graph.
///
///             The most straightforward use-case for that method is incrementing some counter in a thread-safe manner.
///
///             The method returns NO and `error` out parameter in the following cases:
///             (a) there are neither parent object or the default value for it,
///             (b) underlying value store is in trouble to persist it.
///
/// @returns    YES if the updated value was successfully persisted in the store.
///
- (BOOL)updateValueAtKeyPath:(NSString *)keyPath
                   withBlock:(id _Nullable (^)(id _Nullable oldValue, NSError **error))block
                       error:(NSError **)error;

///
/// @brief      Shortcut for forceUpdateValue:atKeyPath:
///
- (void)setObject:(nullable POSLensValue *)value forKeyedSubscript:(NSString *)keyPath;

///
/// @brief      Removes the value from the store.
///
/// @discussion Clients will receive the default value for that setting or nil if the default value doesn't exist.
///
/// @returns    YES if the value was successfully removed from the store.
///
- (BOOL)removeValue:(NSError **)error;

///
/// @brief      Removes the value from memory and tries to remove it from the store.
///
/// @discussion Clients will receive the default value for that setting or nil if the default value doesn't exist.
///
- (void)removeValueAnyway;

@end

#pragma mark -

@protocol POSValueStore;

///
/// Lens factory methods. They contain minimal parameters set for persisting stores.
/// For the lens with advanced options use factory method which receives manually configured store.
/// All methods don't throw exceptions. POSLens factory methods unpack initial value in a try-catch
/// block and repack any exceptions in error out parameter.
///
@interface POSLens (Factory)

///
/// Creates lens with in-memory POSEphemeralValueStore.
///
/// @param value Initial value.
///
+ (instancetype)lensWithValue:(nullable POSLensValue *)value;

///
/// Creates lens with explicitly specified storage.
///
/// @discussion The method is suitable to create lens with manually configured builtin stores,
///             or custom value stores. The method loads initial value from the specified
///             storage and returns an error if something went wrong.
///
/// @param value The default value for the case when provided store doesn't contain any value yet.
/// @param store A prebuilt or user-defined storage service to persist value.
/// @param error An error which occurred during the initial value loading from the storage service.
///
+ (nullable instancetype)lensWithDefaultValue:(nullable POSLensValue *)value
                                        store:(id<POSValueStore>)store
                                       logger:(nullable id<POSLogger>)logger
                                        error:(NSError **)error;

///
/// Creates lens with file-based POSFileValueStore.
///
/// @param value    The default value for the cases when the file at specified path doesn't exist or is empty.
/// @param filePath A path to file for persisting value.
/// @param error    An error which occurred during the initial value loading from the file.
///
+ (nullable instancetype)lensWithDefaultValue:(nullable POSLensValue *)value
                                     filePath:(NSString *)filePath
                                       logger:(nullable id<POSLogger>)logger
                                        error:(NSError **)error;

///
/// Creates lens with keychain-based POSKeychainValueStore.
///
/// @param value    The default value for the case when the keychain doesn't conatain a value at the specified key.
/// @param service  `kSecAttrService` argument in keychain query which represents the service associated with value.
/// @param valueKey `kSecAttrAccount` argument in keychain query which represents value's account name.
/// @param error    An error which occurred during the initial value loading from the file.
///
+ (nullable instancetype)lensWithDefaultValue:(nullable POSLensValue *)value
                              keychainService:(NSString *)service
                                     valueKey:(NSString *)valueKey
                                       logger:(nullable id<POSLogger>)logger
                                        error:(NSError **)error;

///
/// Creates lens with NSUserDefaults-based POSUserDefaultsValueStore.
///
/// @param value        The default value for the case when the userDefaults doesn't conatain a value at the specified key.
/// @param userDefaults NSUserDefaults instance for persisting value. The most popular can be obtained
///                     using `standardUserDefaults` factory method of NSUserDefaults class.
/// @param valueKey     The value identifier in the NSUserDefaults storage.
/// @param error        An error which occurred during the initial value loading from the NSUserDefaults instance.
///
+ (nullable instancetype)lensWithDefaultValue:(nullable POSLensValue *)value
                                 userDefaults:(NSUserDefaults *)userDefaults
                                     valueKey:(NSString *)valueKey
                                       logger:(nullable id<POSLogger>)logger
                                        error:(NSError **)error;

@end

#pragma mark - Shortcuts

//
// Property-safe way to extract sublens from the lens:
// @code POSLens<NSString *> *email = POS_LENS(userLens, email); @endcode
//
#define POS_LENS(LENS, KEYPATH) \
    [LENS lensForKeyPath:@keypath(LENS.value, KEYPATH)]

//
// Property-safe way to update value using the lens:
// @code NSString *email = POS_VALUE(userLens, email); @endcode
//
#define POS_VALUE(LENS, KEYPATH) \
    POS_LENS(LENS, KEYPATH).value

//
// Property-safe way to update value using the lens:
// @code POS_SET_VALUE(userLens, email) = @"inbox@example.com"; @endcode
//
#define POS_SET_VALUE(LENS, KEYPATH) \
    LENS[@keypath(LENS.value, KEYPATH)]

NS_ASSUME_NONNULL_END
