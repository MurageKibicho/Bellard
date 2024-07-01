#include <stdio.h>
#define MAX_TENANT_STRING_LENGTH 100
int numberOfTenants = 0;

typedef struct tenant_details_struct Tenant;
struct tenant_details_struct
{
	char tenantName[MAX_TENANT_STRING_LENGTH];
	char houseNumber[MAX_TENANT_STRING_LENGTH];
	char mobilePhoneNumber[MAX_TENANT_STRING_LENGTH];
	char invoiceDate[MAX_TENANT_STRING_LENGTH];
	char dueDate[MAX_TENANT_STRING_LENGTH];
	char rent[MAX_TENANT_STRING_LENGTH];
	char serviceCharge[MAX_TENANT_STRING_LENGTH];
	char waterBill[MAX_TENANT_STRING_LENGTH];
	char subtotal[MAX_TENANT_STRING_LENGTH];
	char subtotalBeforeTax[MAX_TENANT_STRING_LENGTH];
	char taxRate[MAX_TENANT_STRING_LENGTH];
	char totalTax[MAX_TENANT_STRING_LENGTH];
	char balanceDue[MAX_TENANT_STRING_LENGTH];
};

Tenant tenants[] = 
	{
		{
			.tenantName = "Jane Doe",
			.houseNumber = "124",
			.mobilePhoneNumber = "555-1234",
			.invoiceDate = "2023-06-01",
			.dueDate = "2023-06-15",
			.rent = "1000",
			.serviceCharge = "200",
			.waterBill = "50",
			.subtotal = "1250",
			.subtotalBeforeTax = "1000",
			.taxRate = "0.05",
			.totalTax = "50",
			.balanceDue = "1300"
		},
		{
			.tenantName = "John Doe",
			.houseNumber = "House 1",
			.mobilePhoneNumber = "+254757012296",
			.invoiceDate = "01/07/2024",
			.dueDate = "01/07/2024",
			.rent = "100,000",
			.serviceCharge = "5,000",
			.waterBill = "5000",
			.subtotal = "110,000",
			.subtotalBeforeTax = "102,500",
			.taxRate = "7.50%",
			.totalTax = "7500",
			.balanceDue = "110,000"
		}
		
	};
