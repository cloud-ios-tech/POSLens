//
//  POSPersistentValueStore.m
//  POSLens
//
//  Created by Pavel Osipov on 07/02/2018.
//  Copyright © 2018 Pavel Osipov. All rights reserved.
//

#import "POSPersistentValueStore.h"
#import "NSError+POSL.h"
#import "NSException+POSL.h"

NS_ASSUME_NONNULL_BEGIN

@implementation POSPersistentValueStore

#pragma mark - POSValueStore

- (BOOL)saveValue:(nullable POSLensValue *)value error:(NSError **)error {
    @try {
        if (value == nil) {
            return [self removeData:error];
        }        
        NSMutableData *data = [NSMutableData data];
        NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc ] initForWritingWithMutableData:data];
        [archiver setOutputFormat:NSPropertyListBinaryFormat_v1_0];
        [archiver encodeRootObject:value];
        [archiver finishEncoding];
        return [self saveData:data error:error];
    } @catch (NSException *exception) {
        POSLAssignError(error, [NSError posl_systemErrorWithFormat:exception.reason]);
        return NO;
    }
}

- (nullable POSLensValue *)loadValue:(NSError **)error {
    @try {
        NSData *data = [self loadData:error];
        if (!data) {
            return nil;
        }
        NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];
        POSLensValue *value = [unarchiver decodeObject];
        POSL_CHECK([value conformsToProtocol:@protocol(POSLensPolicy)]);
        POSL_CHECK([value conformsToProtocol:@protocol(NSCopying)]);
        return value;
    } @catch (NSException *exception) {
        POSLAssignError(error, [NSError posl_systemErrorWithFormat:exception.reason]);
        return nil;
    }
}

#pragma mark - Public

- (BOOL)saveData:(NSData *)data error:(NSError **)error {
    return YES;
}

- (nullable NSData *)loadData:(NSError **)error {
    return nil;
}

- (BOOL)removeData:(NSError **)error {
    return YES;
}

@end

NS_ASSUME_NONNULL_END
